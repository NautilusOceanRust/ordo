# 📝 Ordo - Task Manager for the Terminal

Ordo is a simple, lightweight, and customizable task manager designed to be used entirely from your terminal 💻. It is built in C with the ncurses library, focusing on efficiency and ease of use.

### Appearance ✨
```
┌────────────────────────────┐┌──────────────────────────────────────────────────┐
│           📝 Ordo          ││ 📋 Tasks                                         │
│────────────────────────────││                                                  │
│ [1] ✨ Add Task            ││  ○ Create an awesome README                      │
│ [2] 🗑️ To Trash            ││  ✔ Implement the app logic                      │
│ [3] 📝 Edit Task           ││ >○ Add more themes                               │
│ [4] 🔄 Complete/Reopen     ││                                                  │
│ [5] 👀 View Full Task      ││                                                  │
│ [6] ⚙️ Settings            ││                                                  │
│ [u] ↩️ Undo                ││                                                  │
│ [t] 🗑️ View Trash          ││                                                  │
│ [7] 🚪 Exit                ││                                                  │
│                            ││                                                  │
└────────────────────────────┘└──────────────────────────────────────────────────┘
 Task restored successfully! ✨
```

## 🚀 Features

*   **Add, Edit, and Complete Tasks:** Full lifecycle management of your tasks.
*   **Trash and Restore:** Move tasks to the trash instead of permanently deleting them, with the option to restore them later.
*   **Multi-level Undo/Redo:** Undo and redo most actions, such as adding, deleting, editing, and completing tasks.
*   **Detailed View:** View the full description of long tasks.
*   **Internationalization (i18n):** Support for multiple languages. Add your own!
*   **Customizable Themes:** Comes with over 40 pre-installed color themes and allows you to create your own.
*   **Responsive Interface:** The interface adapts to terminal resizing.
*   **Simple Configuration:** Customize the language and theme through a simple configuration file.
*   **Enhanced UI:** The user interface has been improved for a better experience.

## 🏗️ Project Architecture

Ordo is designed with a modular architecture to separate concerns and make the code easier to maintain and extend. The main components are:

-   **`main.c`**: The entry point of the application, responsible for the main loop and event handling.
-   **`ui.c` / `ui.h`**: Manages all aspects of the user interface, including drawing windows, menus, and tasks using the `ncurses` library.
-   **`database.c` / `database.h`**: Handles all interactions with the SQLite database 💾, such as adding, deleting, and querying tasks.
-   **`task_list.c` / `task_list.h`**: An abstraction that manages the list of tasks in memory.
-   **`config.c` / `config.h`**: Manages loading and saving user settings from the `config.ini` file.
-   **`theme.c` / `theme.h`**: Responsible for loading and managing color themes.
-   **`i18n.c` / `i18n.h`**: Handles internationalization by loading language files.
-   **`utils.c` / `utils.h`**: Contains utility functions used throughout the project.
-   **`platform_utils.c` / `platform_utils.h`**: Provides platform-specific functionalities, such as finding configuration directories.

## 🛠️ How to Compile

Ordo uses the **Meson** build system with **Ninja** and can be compiled on Linux, macOS, and Windows.

### 1. Install Dependencies

First, install the necessary development tools and libraries for your operating system.

#### 🐧 Linux

*   **On Debian/Ubuntu:**
    ```bash
    sudo apt-get install build-essential meson ninja-build libsqlite3-dev libncursesw5-dev libcjson-dev
    ```
*   **On Fedora/CentOS/RHEL:**
    ```bash
    sudo dnf groupinstall "Development Tools"
    sudo dnf install meson ninja-build sqlite-devel ncurses-devel cjson-devel
    ```
*   **On Arch Linux:**
    ```bash
    sudo pacman -S base-devel meson ninja sqlite ncurses cjson
    ```

#### 🍎 macOS

The easiest way to install dependencies on macOS is using [Homebrew](https://brew.sh/).

```bash
brew install meson ninja sqlite ncurses cjson
```

#### 🪟 Windows

For Windows, we recommend using the **MSYS2** environment to set up the required toolchain.

1.  **[Install MSYS2](https://www.msys2.org/)** and open the MSYS2 MinGW 64-bit terminal.
2.  Install the toolchain and dependencies using `pacman`:
    ```bash
    pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-meson mingw-w64-x86_64-ninja \
                mingw-w64-x86_64-sqlite3 mingw-w64-x86_64-pdcurses mingw-w64-x86_64-cjson
    ```
    *Note: On Windows, `pdcurses` is used as the replacement for `ncurses`.*


### 2. Compile the Project

Once the dependencies are installed, the compilation steps are the same for all platforms:

```bash
# 1. Configure the build directory (e.g., 'build')
meson setup build

# 2. Compile the project using Ninja
ninja -C build
```

#### Compiling with Clang

If you have Clang installed and want to use it instead of GCC, you can tell Meson to use it by setting the `CC` environment variable during the setup step:

```bash
CC=clang meson setup build
ninja -C build
```

This will create two executables in the `build/` directory:
*   `ordo` (or `ordo.exe` on Windows): The main program.
*   `ordo-importer` (or `ordo-importer.exe` on Windows): A tool to import tasks from a JSON or plain text file into the Ordo database.

### `ordo-importer` 🤖

This tool allows you to import tasks from a JSON or plain text file.

- **JSON format**: The file must contain an array of objects, where each object represents a task and has a `description` (string) and a `completed` (boolean) field.
- **Plain text format**: Each line in the file is treated as a separate task.

**Example `tasks.json` file:**
```json
[
  {
    "description": "My first imported task",
    "completed": false
  },
  {
    "description": "My second imported task, already done",
    "completed": true
  }
]
```

**How to use:**
```bash
# The first argument is the path to the JSON file
# The second argument is the path to the database file
./build/ordo-importer tasks.json ~/.config/ordo/tasks.db
```

## 🕹️ How to Use

To start the program, run the compiled file:

```bash
./build/ordo
```

### Hotkeys

| Key(s)                  | Action                                     | View(s)          |
| ----------------------- | ------------------------------------------ | ---------------- |
| **Up/Down Arrows**      | Navigate the task list                     | Main / Trash     |
| **1**                   | Add a new task                             | Main             |
| **2**                   | Move the selected task to the trash        | Main             |
| **3**                   | Edit the description of the selected task  | Main             |
| **4**                   | Toggle the status (done/pending)           | Main             |
| **5**                   | View the full description of the task      | Main             |
| **6**                   | Open the settings menu                     | Main             |
| **u**                   | Undo the last action                       | Main             |
| **y**                   | Redo the last undone action                | Main             |
| **t**                   | Switch between Main and Trash views        | Main / Trash     |
| **r**                   | Restore the selected task from the trash   | Trash            |
| **d**                   | Permanently delete the selected task       | Trash            |
| **q** / **7**           | Exit the program / Return to the main view | Main / Trash     |

## ⚙️ Configuration

Ordo creates a configuration file at `~/.config/ordo/config.ini`. You can edit this file to customize the application.

*   **Language:** Change the `lang` key to the two-letter code of the desired language (e.g., `lang = en`). The language files are in the `langs/` folder.
*   **Theme:** Change the `theme` key to the name of one of the available themes (e.g., `theme = Dracula`).

### Adding New Themes

You can create your own themes! Create a `.theme` file in the `~/.config/ordo/themes/` directory. Use the following format:

```ini
# ~/.config/ordo/themes/my_theme.theme
name = My Awesome Theme
header_fg = COLOR_MAGENTA
header_bg = COLOR_BLACK
done_fg = COLOR_GREEN
done_bg = COLOR_BLACK
pending_fg = COLOR_WHITE
pending_bg = COLOR_BLACK
error_fg = COLOR_RED
error_bg = COLOR_BLACK
success_fg = COLOR_GREEN
success_bg = COLOR_BLACK
task_done_fg = COLOR_WHITE
task_done_bg = COLOR_BLACK
task_pending_fg = COLOR_WHITE
task_pending_bg = COLOR_BLACK
```

The available colors are: `COLOR_BLACK`, `COLOR_RED`, `COLOR_GREEN`, `COLOR_YELLOW`, `COLOR_BLUE`, `COLOR_MAGENTA`, `COLOR_CYAN`, `COLOR_WHITE`.

## 🤝 Contributions

Contributions are welcome! Feel free to open an *issue* to report bugs or suggest new features. If you wish, you can create a *pull request* with your improvements.

## 📜 License

This project is distributed under the MIT license. See the `LICENSE` file for more details.

