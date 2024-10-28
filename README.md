<link rel="stylesheet" type='text/css' href="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/devicon.min.css" />

# Introduction

Developing well-known games in various languages.

# Game List

- **[Partially Done]** <i class="devicon-python-plain"></i> Sudoku
- **[Partially Done]** <i class="devicon-c-plain"></i> Tetris
- **[In Process]** <i class="devicon-rust-original"></i> Chess
- **[In Process]** <i class="devicon-cplusplus-plain"></i> Minesweeper
- **[Not Started]** Flappy Bird
- **[Not Started]** Snake Game
- **[Not Started]** Puyo Puyo

# Setup Environments

## Python

> Install Python from **[python.org](https://www.python.org/downloads/)**

### Virtual Environment

- Setup Virtual Environment

```bash
python -m venv ${VENV_NAME}
```

- Activate Virtual Environment

```bash
source ${PATH_TO_BIN}/activate
```

- Deactivate Virtual Environment

```bash
deactivate
```

### Dependencies

- Export Dependencies to requirements.txt

```bash
pip freeze > ${FILENAME}
```

- Install Dependencies in requirements.txt

```bash
pip install -r ${FILENAME}
```

## C/C++

> **Macos**  
> You do not need any additional settings.
>
> **Windows / Linux**  
> `gcc`/`g++`, `make` needed.

### Compile and Run

```bash
cd ${PATH_TO_MAKEFILE}
make
```

### Clean build files

```bash
cd ${PATH_TO_MAKEFILE}
make clean
```

## Rust

> See **[rust-lang.org](https://www.rust-lang.org/tools/install)** to setup rust environment.

To be updated...
