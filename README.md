# shell
  Shell is a command interpreter used in operating systems of the Unix family, in which the user can either issue commands to the operating system individually, or run scripts consisting of a list of commands. Working through the shell looks like this: the user enters a command, presses Enter, the system executes the command, writes the execution result to the screen, and again waits for the next command to be entered.
## Stages:
1. Divide into lexemes
2. Standard launch of the program
3. I / O redirection
4. Conveyor for two elements
5. Conveyor for any amount
6. Background mode
7. Change the cd.
8. New &&.
9. Ctrl+c

## Запуск
* Download repository:
```bash
git clone https://github.com/abdrimm/shell
```
* Compile by [Makefile](https://github.com/abdrimm/shell/blob/master/Makefile):
```bash
make
```
* Execute:
```bash
./main
```
* For example:
```bash
ls | sort | grep
```
* At the end:
```bash
exit
```
* or
```bash
quit
```
