[![progress-banner](https://backend.codecrafters.io/progress/shell/b32f75ea-fd02-4beb-b67c-f80935c9c241)](https://app.codecrafters.io/users/codecrafters-bot?r=2qF)

This is a completed C solution for the
["Build Your Own Shell" Challenge](https://app.codecrafters.io/courses/shell/overview), plus some extra features I added later.

In this challenge, you'll build your own POSIX compliant shell that's capable of
interpreting shell commands, running external programs and builtin commands like
cd, pwd, echo and more. Along the way, you'll learn about shell command parsing,
REPLs, builtin commands, and more.

**Note**: If you're viewing this repo on GitHub, head over to
[codecrafters.io](https://codecrafters.io) to try the challenge.

# Features

- Cross-platform support (Linux/macOS via `fork`/`exec`, Windows via `_spawnv`).
- Interactive REPL with a `$ ` prompt.
- Builtins: `echo`, `exit`, `type`, `pwd`, `cd`, `history`, `cshell`.
- External command execution via `PATH` lookup, with a fallback to current directory.
- Command parsing with:
   - Single and double quotes.
   - Backslash escaping (context-aware inside/outside quotes).
- Pipeline support (`|`) on non-Windows platforms (multi-stage pipelines).
- Redirection support:
   - `>` / `1>` for stdout overwrite
   - `>>` / `1>>` for stdout append
   - `2>` for stderr overwrite
   - `2>>` for stderr append
- History support on non-Windows platforms:
   - Persistent history load/save via `HISTFILE`.
   - `history -r <file>`, `history -w <file>`, `history -a <file>`.
   - `history [N]` to show the last N entries.
- Tab autocomplete:
   - Linux/macOS: Readline completion for builtins and `PATH` executables.
   - Windows: Custom TAB completion for builtins and `PATH` executables.
- ANSI color support for Windows terminals.
- Startup splash screen with randomized ASCII art and rotating status messages.

# Challenge status

This repository contains a completed solution for the CodeCrafters “Build Your Own Shell” challenge.

# Running locally

1. Ensure you have `cmake` installed locally.
1. Run `./your_program.sh` to build and run the shell (implemented in `src/main.c`).
