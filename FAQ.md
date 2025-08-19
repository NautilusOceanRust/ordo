# 🧐 FAQ - Frequently Asked Questions

This page answers some common questions about Ordo.

---

### 🛠️ Compilation and Installation

**Q: How do I compile the project?**

A: The project uses Meson and Ninja. Detailed instructions are in the [How to Compile](README.md#how-to-compile) section of the main README.md file.

**Q: I'm getting an error about `ncursesw` not being found during compilation.**

A: This means that the `ncurses` development library with support for "wide" characters (Unicode) is not installed. Make sure you have installed the correct package for your system (e.g., `libncursesw5-dev` on Debian/Ubuntu or `ncurses-devel` on Fedora).

**Q: Can I install Ordo with `apt`, `dnf`, or `brew`?**

A: Currently, Ordo is not distributed through package managers. You need to compile it from source.

**Q: Does Ordo work on Windows or macOS?**

A: The project was written with portability in mind, using code that adapts to the platform. Although the main development focus is Linux, it should be compilable on other systems with the correct dependencies (such as `pdcurses` on Windows). Small adjustments to the `meson.build` file may be necessary to find the libraries on these systems.

---

### 🚀 Usage and Features

**Q: What is the `ordo-importer` executable that is created along with `ordo`?**

A: The `ordo-importer` is a command-line tool for bulk importing tasks from a JSON file into the Ordo database. The JSON file must be an array of objects, where each object has a `description` (string) and a `completed` (boolean) field.

*   **Usage:** `./build/ordo-importer <input_file.json> <path_to_db>`
*   **Example:** `./build/ordo-importer my_tasks.json ~/.config/ordo/tasks.db`

**Q: Where are the configuration and data files located?**

A: The location depends on your operating system, but on Linux, they are typically found in:
*   **Configuration and Database:** The `config.ini` and `tasks.db` files are located in `~/.config/ordo/`.
*   **Themes and Languages:** Custom themes and language files can be placed in `~/.config/ordo/themes/` and `~/.config/ordo/langs/` respectively.

**Q: How do I create a new color theme?**

A: Create a `.theme` file (e.g., `my_theme.theme`) in your themes configuration directory (`~/.config/ordo/themes/`). The `README.md` has a detailed section on [how to add new themes](README.md#adding-new-themes) with all the colors you can define.

**Q: How do I add a new language?**

A: Go to the `langs/` directory in the source code, copy an existing file (e.g., `en.lang` to `fr.lang`), and translate the key values. To use your new language, edit the `config.ini` file and change the `lang` key to your language code (e.g., `lang = fr`).

---

### 🐛 Troubleshooting

**Q: I accidentally deleted a task. Can I recover it?**

A: Yes! Ordo has a multi-level Undo/Redo system and a trash.
*   **Undo/Redo:** You can press `u` to undo most actions (add, delete, edit, complete) and `y` to redo them.
*   **Trash:** If you don't want to undo, you can view all deleted tasks by pressing `t`. In the trash view, you can restore a task with `r` or permanently delete it with `d`.

**Q: Accents, emojis, or other special characters do not appear correctly.**

A: This is usually a terminal configuration issue, not an Ordo issue. Ordo uses `ncursesw` and sets the `locale` to support UTF-8. Make sure your terminal is configured to use UTF-8 encoding and that you are using a font that supports the desired characters.

---

### 👨‍💻 For Developers

**Q: What is the database schema?**

A: The database is a simple SQLite file (`tasks.db`) with a single main table named `tasks`. The schema is as follows:
```sql
CREATE TABLE tasks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    description TEXT NOT NULL,
    completed INTEGER NOT NULL DEFAULT 0,
    is_deleted INTEGER NOT NULL DEFAULT 0
);
```
-   `description`: The text of the task.
-   `completed`: An integer that acts as a boolean (`0` for pending, `1` for completed).
-   `is_deleted`: An integer that acts as a boolean (`0` for active, `1` for in the trash).

**Q: How does the Undo/Redo system work?**

A: The Undo/Redo functionality is implemented using two stacks (one for undo, one for redo) that store `Command` objects. Each `Command` represents an action (like adding, deleting, or editing a task) and contains enough information to reverse that action. When a user performs an action, a corresponding command is pushed onto the undo stack. When they press `u`, the command is popped, its `undo` function is executed, and the command is moved to the redo stack.

**Q: How can I contribute to the project?**

A: Contributions are very welcome! The best way to start is by checking the `README.md` file to understand the project's architecture. You can open an *issue* to report bugs or suggest new features. If you want to contribute with code, feel free to create a *pull request* with your improvements. All contributions are appreciated!