[![progress-banner](https://backend.codecrafters.io/progress/shell/b32f75ea-fd02-4beb-b67c-f80935c9c241)](https://app.codecrafters.io/users/codecrafters-bot?r=2qF)

This is a completed C solution for the
["Build Your Own Shell" Challenge](https://app.codecrafters.io/courses/shell/overview), plus some extra features I added later.

In this challenge, you'll build your own POSIX compliant shell that's capable of
interpreting shell commands, running external programs and builtin commands like
cd, pwd, echo and more. Along the way, you'll learn about shell command parsing,
REPLs, builtin commands, and more.

**Note**: If you're viewing this repo on GitHub, head over to
[codecrafters.io](https://codecrafters.io) to try the challenge.

https://github.com/user-attachments/assets/5c94e22a-642a-4c0e-bade-804216c247e6

# Features

- Cross-platform support (Linux/macOS via `fork`/`exec`, Windows via `_spawnv`).
- Interactive REPL with a `$ ` prompt.
- Built-in commands and utilities (see list below).
- External command execution via `PATH` lookup, with a fallback to the current directory.
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

# Commands

Alphabetical list of supported commands:

- `bindump` — View a file in binary.
- `cd <dir>` — Change directory.
- `clear` / `cls` — Clear the terminal.
- `cshell` — Show the theme gallery.
- `echo <txt>` — Print text.
- `exit` — Close the shell.
- `help` — Show built-in help.
- `hexdump` — View a file in hex.
- `history` — Show or manage history (`-r`, `-w`, `-a`, or a numeric limit).
- `ip` — Show network info.
- `ls [-a]` — List files (use `-a` for hidden).
- `mx <domain>` — Find mail servers for a domain.
- `ping [host]` — Ping a host (defaults to 8.8.8.8).
- `pwd` — Print working directory.
- `type <cmd>` — Identify builtin or executable path.
- `weather` — Get live weather report.
- `whoami` — Show current user.

# Challenge status

This repository contains a completed solution for the CodeCrafters “Build Your Own Shell” challenge.

# Running locally

1. Ensure you have `cmake` installed locally.
1. Run `./your_program.sh` to build and run the shell (implemented in `src/main.c`).
