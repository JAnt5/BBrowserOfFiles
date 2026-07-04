#include <curses.h>
#include <dirent.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <term.h>
#include <unistd.h>

#define MAX_FILE_NAME 255

void file_menu(WINDOW *menu_win, int highlight, int x, int y, int n_choices,
               char *choices[]) {
  int i;
  x += 2;
  y += 2;

  box(menu_win, 0, 0);
  for (i = 0; i < n_choices; ++i) {
    if (highlight == i + 1) /* High light the present choice */
    {
      wattron(menu_win, A_REVERSE);
      mvwprintw(menu_win, y, x, "%s", choices[i]);
      wattroff(menu_win, A_REVERSE);
    } else
      mvwprintw(menu_win, y, x, "%s", choices[i]);
    ++y;
  }
  wrefresh(menu_win);
}

int main() {

  initscr();
  int xMax, yMax;
  getmaxyx(stdscr, yMax, xMax);
  WINDOW *window = newwin(yMax, xMax, 0, 0);
  keypad(window, TRUE);

  box(window, 0, 0);

  refresh();
  char dir[256] = "";
  struct dirent *de;
  DIR *dr;

  int x, y, file_count, highlight = 1, choice = 0, c;
  while (1) {
    highlight = 1;
    choice = 0;
    wclear(window);

    wrefresh(window);
    x = 2;
    y = 2;
    if (chdir(dir) != 0) {
    }

    mvwprintw(window, y, x, "Current working directory: %s", getcwd(dir, 100));

    dr = opendir(dir);
    if (dr == NULL) {
      mvwprintw(window, y, x + 50, "Could not open current directory");
      wrefresh(window);
      return 0;
    }

    file_count = 0;
    while ((de = readdir(dr)) != NULL) {
      file_count++;
    }
    rewinddir(dr);

    // Dynamic memory allocation and array
    char **file_list = NULL;
    file_list = (char **)malloc(file_count * sizeof(char *));
    for (int i = 0; i < file_count; i++) {
      file_list[i] = (char *)malloc((MAX_FILE_NAME + 1) * sizeof(char));
      if (file_list[i] == NULL) {
        mvwprintw(window, y, x + 50, "Could not allocate memory");
        wrefresh(window);

        return 0;
      }
      // Making a lsit
      if ((de = readdir(dr)) != NULL) {
        strcpy(file_list[i], de->d_name);
      }
    }
    // za ponovni read
    file_menu(window, highlight, x, y, file_count, file_list);
    while (1) {
      c = wgetch(window);

      switch (c) {
      case KEY_UP:
        if (highlight == 1)
          highlight = file_count;
        else
          --highlight;
        break;
      case KEY_DOWN:
        if (highlight == file_count)
          highlight = 1;
        else
          ++highlight;
        break;
      case KEY_BACKSPACE:
        break;
      case 10:
        choice = highlight;
        break;
      default:
        break;
        mvwprintw(window, y, x + 30, "wrong char bro");
        wrefresh(window);
      }
      file_menu(window, highlight, x, y, file_count, file_list);
      if (choice != 0) {
        break;
      }
    }
    strcpy(dir,file_list[choice-1]);

    // freeing memory
    for (int i = 0; i < file_count; i++) {
      free(file_list[i]);
    }
    // Free memory for the array of pointers
    free(file_list);
    closedir(dr);
  }
  //getch();
  endwin();

  return 0;
}