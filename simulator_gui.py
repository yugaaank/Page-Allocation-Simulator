
import tkinter as tk
from tkinter import ttk, messagebox, scrolledtext
from collections import deque
import sys

# -----------------------------------------------------------------------------
# LOGIC: Page Replacement Algorithms
# -----------------------------------------------------------------------------

def solve_fifo(pages, capacity):
    """
    First-In-First-Out (FIFO) Page Replacement.
    Returns: (faults, hits, simulation_steps)
    simulation_steps is a list of dicts: {'page': int, 'frames': list, 'status': str}
    """
    frames = []
    faults = 0
    hits = 0
    steps = []
    
    # We use a simple list to simulate the frames. 
    # For FIFO, we can just replace frames[pointer] and increment pointer cyclicly,
    # or pop(0) and append. 
    # Let's use a deque-like approach or just a list with a replacement index.
    
    # Using a list + queue for order
    q = deque()
    in_memory = set()
    
    for page in pages:
        status = ""
        if page in in_memory:
            hits += 1
            status = "HIT"
        else:
            faults += 1
            status = "FAULT"
            if len(frames) < capacity:
                frames.append(page)
                in_memory.add(page)
                q.append(page)
            else:
                # Evict the oldest
                victim = q.popleft()
                if victim in in_memory:
                    in_memory.remove(victim)
                
                # Replace in frames list (to maintain visual consistency if we want connection to slots)
                # But typically frames are unordered in set, ordered in queue. 
                # For visualization, we just show the current state of frames.
                # To be precise with "slots", we'd need to track indices. 
                # Simplest for display: just show the list content.
                # If we want to replace the exact slot, let's look for victim index.
                if victim in frames:
                    idx = frames.index(victim)
                    frames[idx] = page
                else:
                    # Should not happen if logic is correct
                    frames.append(page) 
                
                in_memory.add(page)
                q.append(page)
        
        steps.append({
            'page': page,
            'frames': list(frames), # copy
            'status': status
        })
        
    return faults, hits, steps

def solve_lru(pages, capacity):
    """
    Least Recently Used (LRU) Page Replacement.
    """
    frames = []
    faults = 0
    hits = 0
    steps = []
    
    # We need to track recent usage.
    # An ordered list where the end is most recently used.
    
    # However, to maintain "slots" for visualization (like 3 frames staying in same visual slots mostly),
    # pure LRU stack reshuffles everything. 
    # Standard visualization usually just lists current pages.
    # We'll stick to a simple list for frames.
    
    usage_history = [] # functions as the stack
    
    for page in pages:
        status = ""
        if page in frames:
            hits += 1
            status = "HIT"
            # Update usage
            usage_history.remove(page)
            usage_history.append(page)
        else:
            faults += 1
            status = "FAULT"
            if len(frames) < capacity:
                frames.append(page)
                usage_history.append(page)
            else:
                # Evict LRU
                victim = usage_history.pop(0) # input at end, remove from start
                # Replace in frames
                idx = frames.index(victim)
                frames[idx] = page
                
                usage_history.append(page)
        
        steps.append({
            'page': page,
            'frames': list(frames),
            'status': status
        })

    return faults, hits, steps

def solve_optimal(pages, capacity):
    """
    Optimal Page Replacement.
    """
    frames = []
    faults = 0
    hits = 0
    steps = []
    
    for i, page in enumerate(pages):
        status = ""
        if page in frames:
            hits += 1
            status = "HIT"
        else:
            faults += 1
            status = "FAULT"
            if len(frames) < capacity:
                frames.append(page)
            else:
                # Find the page in frames that will not be used for longest time
                farthest_idx = -1
                victim_frame_idx = -1
                
                for f_idx, f_page in enumerate(frames):
                    next_use = float('inf')
                    # check future
                    for k in range(i + 1, len(pages)):
                        if pages[k] == f_page:
                            next_use = k
                            break
                    
                    if next_use > farthest_idx:
                        farthest_idx = next_use
                        victim_frame_idx = f_idx
                
                frames[victim_frame_idx] = page
        
        steps.append({
            'page': page,
            'frames': list(frames),
            'status': status
        })

    return faults, hits, steps

# -----------------------------------------------------------------------------
# GUI
# -----------------------------------------------------------------------------

class SimulatorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Page Replacement Simulator")
        self.root.geometry("800x600")
        
        # Styles
        style = ttk.Style()
        style.configure("TButton", font=("Helvetica", 10), padding=5)
        style.configure("TLabel", font=("Helvetica", 11))
        style.configure("Header.TLabel", font=("Helvetica", 14, "bold"))

        # Main Container
        main_frame = ttk.Frame(root, padding="20")
        main_frame.pack(fill=tk.BOTH, expand=True)

        # Header
        header = ttk.Label(main_frame, text="Page Replacement Algorithm Simulator", style="Header.TLabel")
        header.pack(pady=(0, 20))

        # Input Area
        input_frame = ttk.LabelFrame(main_frame, text="Configuration", padding="15")
        input_frame.pack(fill=tk.X, pady=(0, 20))

        # Frames Input
        frame_box = ttk.Frame(input_frame)
        frame_box.pack(fill=tk.X, pady=5)
        ttk.Label(frame_box, text="Number of Frames:", width=20).pack(side=tk.LEFT)
        self.frames_var = tk.StringVar(value="3")
        self.frames_entry = ttk.Entry(frame_box, textvariable=self.frames_var, width=10)
        self.frames_entry.pack(side=tk.LEFT)

        # Ref String Input
        ref_box = ttk.Frame(input_frame)
        ref_box.pack(fill=tk.X, pady=5)
        ttk.Label(ref_box, text="Reference String:", width=20).pack(side=tk.LEFT)
        self.ref_var = tk.StringVar(value="7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1")
        self.ref_entry = ttk.Entry(ref_box, textvariable=self.ref_var, width=50)
        self.ref_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(0, 10))
        ttk.Button(ref_box, text="Clear", command=lambda: self.ref_var.set("")).pack(side=tk.RIGHT)

        # Buttons
        btn_frame = ttk.Frame(main_frame)
        btn_frame.pack(fill=tk.X, pady=(0, 20))
        
        ttk.Button(btn_frame, text="Run FIFO", command=lambda: self.run_sim("FIFO")).pack(side=tk.LEFT, padx=5)
        ttk.Button(btn_frame, text="Run LRU", command=lambda: self.run_sim("LRU")).pack(side=tk.LEFT, padx=5)
        ttk.Button(btn_frame, text="Run Optimal", command=lambda: self.run_sim("Optimal")).pack(side=tk.LEFT, padx=5)
        ttk.Button(btn_frame, text="Run All (Compare)", command=self.run_all).pack(side=tk.LEFT, padx=5)

        # Output Area
        output_labelframe = ttk.LabelFrame(main_frame, text="Simulation Results", padding="10")
        output_labelframe.pack(fill=tk.BOTH, expand=True)
        
        self.output_text = scrolledtext.ScrolledText(output_labelframe, font=("Courier", 10), state='disabled')
        self.output_text.pack(fill=tk.BOTH, expand=True)

    def log(self, text, end="\n"):
        self.output_text.configure(state='normal')
        self.output_text.insert(tk.END, text + end)
        self.output_text.configure(state='disabled')
        self.output_text.see(tk.END)

    def clear_log(self):
        self.output_text.configure(state='normal')
        self.output_text.delete(1.0, tk.END)
        self.output_text.configure(state='disabled')

    def get_inputs(self):
        try:
            frames = int(self.frames_var.get())
            if frames <= 0:
                raise ValueError
        except ValueError:
            messagebox.showerror("Error", "Number of frames must be a positive integer.")
            return None, None

        raw_ref = self.ref_var.get().strip()
        if not raw_ref:
            messagebox.showerror("Error", "Reference string cannot be empty.")
            return None, None
            
        try:
            # handle spaces, commas
            cleaned = raw_ref.replace(",", " ")
            pages = [int(x) for x in cleaned.split()]
        except ValueError:
            messagebox.showerror("Error", "Reference string must contain only integers separated by spaces.")
            return None, None
            
        return frames, pages

    def format_table(self, algo_name, frames, steps, faults, hits):
        
        total = len(steps)
        ratio = (hits / total * 100) if total > 0 else 0.0

        out = []
        out.append(f"\n{'='*60}")
        out.append(f" Algorithm: {algo_name}")
        out.append(f" Total Frames: {frames}")
        out.append(f"{'='*60}")
        out.append(f"{'Step':<6} | {'Page':<6} | {'Frames State':<30} | {'Status':<10}")
        out.append(f"{'-'*60}")

        for i, step in enumerate(steps):
            f_str = str(step['frames'])
            s_str = step['status']
            out.append(f"{i+1:<6} | {step['page']:<6} | {f_str:<30} | {s_str:<10}")

        out.append(f"{'-'*60}")
        out.append(f"SUMMARY -> Faults: {faults} | Hits: {hits} | Hit Ratio: {ratio:.2f}%")
        out.append(f"{'='*60}\n")
        
        return "\n".join(out)

    def run_sim(self, algo):
        capacity, pages = self.get_inputs()
        if capacity is None: return

        self.clear_log()
        
        faults, hits, steps = 0, 0, []
        
        if algo == "FIFO":
            faults, hits, steps = solve_fifo(pages, capacity)
        elif algo == "LRU":
            faults, hits, steps = solve_lru(pages, capacity)
        elif algo == "Optimal":
            faults, hits, steps = solve_optimal(pages, capacity)
            
        self.log(self.format_table(algo, capacity, steps, faults, hits))

    def run_all(self):
        capacity, pages = self.get_inputs()
        if capacity is None: return

        self.clear_log()
        
        results = []
        
        # FIFO
        f, h, s = solve_fifo(pages, capacity)
        self.log(self.format_table("FIFO", capacity, s, f, h))
        results.append(("FIFO", f, h))
        
        # LRU
        f, h, s = solve_lru(pages, capacity)
        self.log(self.format_table("LRU", capacity, s, f, h))
        results.append(("LRU", f, h))
        
        # Optimal
        f, h, s = solve_optimal(pages, capacity)
        self.log(self.format_table("Optimal", capacity, s, f, h))
        results.append(("Optimal", f, h))
        
        # Comparison
        self.log("\n" + "="*30)
        self.log(" COMPARISON SUMMARY")
        self.log("="*30)
        self.log(f"{'Algorithm':<10} | {'Faults':<8} | {'Hits':<8}")
        self.log("-" * 30)
        for name, f, h in results:
            self.log(f"{name:<10} | {f:<8} | {h:<8}")
        self.log("="*30 + "\n")

if __name__ == "__main__":
    root = tk.Tk()
    app = SimulatorApp(root)
    root.mainloop()
