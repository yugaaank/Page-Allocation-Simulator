<div align="center">

# 🧠 Page-Allocation-Simulator

**Visualize how operating systems allocate memory pages — CLI + GUI.**

[![Stack](https://img.shields.io/badge/stack-C%20%2B%20Python-8b5cf6?style=for-the-badge)](#)
[![Status](https://img.shields.io/badge/status-stable-8b5cf6?style=for-the-badge)](#)
[![PRs](https://img.shields.io/badge/PRs-welcome-8b5cf6?style=for-the-badge)](#contributing)

</div>

---

<div align="center">

| | |
|---|---|
| 🎯 **Purpose** | Simulate page-allocation / replacement strategies |
| 🧩 **Stack** | C++ (core) · Python (GUI) |
| 🌑 **Theme** | Dark / rich |
| 📦 **Status** | Done |

</div>

---

## ✨ Features

- ⚙️ **C++ engine** (`simulator.cpp`) — the allocation logic
- 🖥️ **Python GUI** (`simulator_gui.py`) — watch allocations step by step
- 🎓 Great for OS / memory-management coursework

## 🚀 Quick start

```bash
g++ simulator.cpp -o simulator && ./simulator     # CLI
python simulator_gui.py                            # GUI
```

## 📁 Structure

```
Page-Allocation-Simulator/
├── simulator.cpp       # core simulation
├── simulator_gui.py    # GUI wrapper
└── simulator/          # support / build
```

## 🤝 Contributing

PRs welcome — match the dark/rich README style.

## 📜 License

MIT © Yugank Rathore
