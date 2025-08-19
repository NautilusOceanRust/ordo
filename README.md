# 📝 Ordo - Your Tasks, Your Terminal, Your Way

Tired of heavy, cluttered GUI applications for managing your tasks? Ordo brings task management back to its roots, offering a simple, beautiful, and powerful experience directly in your terminal 💻. Built in C with the ncurses library, Ordo is incredibly fast, light on resources, and designed for those who live and breathe the command line.

Take back control of your to-do list with an interface that's as efficient as you are.

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

## 🌟 Why Choose Ordo?

| Feature                | Description                                                                                                                            |
| :--------------------- | :------------------------------------------------------------------------------------------------------------------------------------- |
| **Blazing Fast & Light** | Written in pure C, Ordo launches instantly and runs smoothly, even on older hardware. No bloat, no lag—just pure productivity.          |
| **Keyboard-First Design**| Navigate, create, and manage tasks without ever leaving your keyboard. It's built for speed and efficiency, keeping you in the flow. |
| **Simple and Intuitive** | No complex commands to memorize. The interface is clean, the hotkeys are visible, and you can get started in seconds.                 |
| **Safe and Forgiving**   | With a trash system and multi-level undo/redo, you can manage your tasks fearlessly. Accidental deletion is a thing of the past.     |
| **Make It Your Own**     | Customize your experience with a wide range of themes and support for multiple languages. Make Ordo truly fit your style.             |
| **Cross-Platform**       | Ordo runs on Linux, macOS, and Windows, providing a consistent experience everywhere you work.                                         |

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

#### Compiling with Clang/LLVM

If you have Clang installed and want to use it instead of GCC, you can tell Meson to use it by setting the `CC` environment variable. This is also useful for developers on macOS, where Clang is the default compiler.

```bash
# Set CC to 'clang' before running meson setup
CC=clang meson setup build

# Compile as usual
ninja -C build
```

If you are on Windows and using the MSYS2 environment, you can install Clang with `pacman -S mingw-w64-x86_64-clang` and follow the same steps.

#### Advanced: Compiling on Windows (Native with Clang-cl)

It is also possible to build a fully native Windows executable using Clang in MSVC-compatible mode (`clang-cl.exe`) and the Visual Studio Build Tools, without relying on MSYS2/MinGW. This method is more complex but produces a binary that links against the native Windows C runtime.

**Prerequisites:**
1.  **Visual Studio Build Tools:** Install from the [Visual Studio website](https://visualstudio.microsoft.com/downloads/), including the "Desktop development with C++" workload.
2.  **LLVM/Clang:** Install from the [official LLVM website](https://releases.llvm.org/download.html).
3.  **`vcpkg` Package Manager:** This is the recommended way to manage native dependencies.
    ```powershell
    git clone https://github.com/microsoft/vcpkg.git
    .\vcpkg\bootstrap-vcpkg.bat
    .\vcpkg\vcpkg integrate install
    ```

**Steps:**

1.  **Open the Developer Command Prompt:** All subsequent commands must be run in a **"x64 Native Tools Command Prompt for VS"**.

2.  **Install Dependencies:** Use `vcpkg` to install the required libraries.
    ```bash
    vcpkg install pdcurses:x64-windows sqlite3:x64-windows cjson:x64-windows
    ```

3.  **Configure Meson:** Navigate to the project directory and run Meson setup, pointing it to the `vcpkg` toolchain file.
    ```bash
    # Replace <path\to\vcpkg> with the actual path to your vcpkg installation
    meson setup build --toolchain <path\to\vcpkg>\scripts\buildsystems\vcpkg.cmake
    ```

4.  **Set Clang-cl as the Compiler:** Reconfigure the build directory to use `clang-cl`.
    ```bash
    meson configure build -D c=clang-cl -D cpp=clang-cl
    ```

5.  **Compile:** Run Ninja as usual.
    ```bash
    ninja -C build
    ```

This will create two executables in the `build/` directory:
*   `ordo` (or `ordo.exe` on Windows): The main program.
*   `ordo-importer` (or `ordo-importer.exe` on Windows): A tool to import tasks from a JSON or plain text file into the Ordo database.

## 💻 Development with VS Code
```

This project is pre-configured for development with Visual Studio Code.

### Prerequisites

1.  **[Install VS Code](https://code.visualstudio.com/)**.
2.  Install the recommended extensions:
    *   `ms-vscode.cpptools`: C/C++ language support.
    *   `ms-vscode.mesonbuild`: Meson language support.

### How to Build and Debug

The repository includes pre-configured build tasks and a debug launch profile.

*   **To Build:**
    1.  Open the Command Palette (`Ctrl+Shift+P` or `Cmd+Shift+P`).
    2.  Type and select **`Tasks: Run Build Task`**.
    3.  Choose **`Meson: Build`**. This will compile the project into the `build/` directory.

*   **To Debug:**
    1.  Set a breakpoint in the code (e.g., in `main.c`).
    2.  Go to the **Run and Debug** view (the bug icon on the sidebar).
    3.  Select **`Debug Ordo`** from the dropdown menu and press the green play button (F5).

    The debugger will automatically build the project (if needed) and launch `ordo` in an external terminal, attaching the debugger to it.

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

The second argument to the importer is the path to the database file. This path varies by operating system.

*   **Linux:** `~/.local/share/ordo/tasks.db`
*   **macOS:** `~/.local/share/ordo/tasks.db`
*   **Windows:** `%LOCALAPPDATA%\ordo\tasks.db` (e.g., `C:\Users\YourUser\AppData\Local\ordo\tasks.db`)

```bash
# Example on Linux/macOS
./build/ordo-importer tasks.json ~/.local/share/ordo/tasks.db

# Example on Windows (Command Prompt)
.\build\ordo-importer.exe tasks.json %LOCALAPPDATA%\ordo\tasks.db
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

Ordo stores its configuration files, database, and themes in standard user directories that vary by operating system.

| OS      | Path                                       | Description                                |
| :------ | :----------------------------------------- | :----------------------------------------- |
| Linux   | `~/.config/ordo/`                          | Config (`config.ini`) and themes (`.theme`) |
|         | `~/.local/share/ordo/`                     | Database (`tasks.db`)                      |
| macOS   | `~/.config/ordo/`                          | Config (`config.ini`) and themes (`.theme`) |
|         | `~/.local/share/ordo/`                     | Database (`tasks.db`)                      |
| Windows | `%APPDATA%\ordo\`                          | Config (`config.ini`) and themes (`.theme`) |
|         | `%LOCALAPPDATA%\ordo\`                     | Database (`tasks.db`)                      |

*   On Windows, `%APPDATA%` typically resolves to `C:\Users\<YourUser>\AppData\Roaming`.
*   On Windows, `%LOCALAPPDATA%` typically resolves to `C:\Users\<YourUser>\AppData\Local`.

You can customize the application by editing `config.ini`.

*   **Language:** Change the `lang` key to the two-letter code of the desired language (e.g., `lang = en`). The language files are in the `langs/` folder.
*   **Theme:** Change the `theme` key to the name of one of the available themes (e.g., `theme = Dracula`).

### Adding New Themes

You can create your own themes! Create a `.theme` file inside the configuration directory for your OS (in a subfolder named `themes`). For example, on Linux, the path would be `~/.config/ordo/themes/my_theme.theme`.

Use the following format:

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

