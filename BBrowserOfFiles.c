#include <curses.h>
#include <dirent.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <term.h>
#include <unistd.h>

#define MAX_FILE_NAME 255

int sdfilt(const struct dirent *de);

void file_menu(WINDOW *menu_win, int highlight, int x, int y, int n_choices,
               struct dirent *choices[]) {
  int i;
  x += 2;
  y += 2;
  char decorativeChar;
  box(menu_win, 0, 0);
  for (i = 0; i < n_choices; ++i) {
    if(choices[i]->d_type == DT_DIR){
      decorativeChar = '>';
    }else{
      decorativeChar = '-';
    }
    if (highlight == i + 1) {
      wattron(menu_win, A_REVERSE);
      mvwprintw(menu_win, y, x, "%c %s",decorativeChar,choices[i]->d_name);

      wattroff(menu_win, A_REVERSE);
    } else{
      mvwprintw(menu_win, y, x, "%c %s",decorativeChar,choices[i]->d_name);

    }

    ++y;
  }
  wattron(menu_win, COLOR_PAIR(0));

  wrefresh(menu_win);
}

int main(int argc, char *argv[]) {

  initscr();
  start_color();
  char dir[256];

  // check if there is a path in the commandline argument
  if (argc <= 1) {
    strcpy(dir, "");
  } else {
    strcpy(dir, argv[argc - 1]);
  }
  // constructs a path to the media folder
  char *username = getlogin();
  char ext_media_path[50] = "/media/";
  strcat(ext_media_path, username);

  // struct dirent *de;
  struct dirent **namelist = NULL;
  size_t entIter = 0;

  int x, y, dir_entry_count, highlight = 1, choice = 0, c;
  bool back, ext_media, resize;
  while (1) {
    back = false;
    ext_media = false;
    resize = false;
    // folder highlighting
    init_pair(1, COLOR_BLACK, COLOR_CYAN);

    int xMax, yMax;
    getmaxyx(stdscr, yMax, xMax);

    refresh();
    WINDOW *window = newwin(yMax, xMax, 0, 0);
    keypad(window, TRUE);

    highlight = 1;
    choice = 0;
    wclear(window);

    wrefresh(window);
    box(window, 0, 0);

    x = 2;
    y = 2;
    if (chdir(dir) != 0) {
    }

    mvwprintw(window, y, x, "Current working directory: %s", getcwd(dir, 100));

    if ((dir_entry_count = scandir(dir, &namelist, sdfilt, alphasort)) < 0) {
      perror("scandir"); 
      return 1;
    }

    file_menu(window, highlight, x, y, dir_entry_count, namelist);
    while (1) {
      c = wgetch(window);

      switch (c) {
      case KEY_UP:
        if (highlight == 1)
          highlight = dir_entry_count;
        else
          --highlight;
        break;
      case KEY_DOWN:
        if (highlight == dir_entry_count)
          highlight = 1;
        else
          ++highlight;
        break;
      case KEY_LEFT:
        back = true;
        break;
      case KEY_BACKSPACE:
        ext_media = true;
        break;
      case KEY_RESIZE:
        wrefresh(window);
        refresh();
        resize = true;
        break;
      case 10:
        choice = highlight;
        break;
      default:
        break;
        mvwprintw(window, y, x + 30, "wrong char bro");
        wrefresh(window);
      }
      file_menu(window, highlight, x, y, dir_entry_count, namelist);
      if (choice != 0 || back || ext_media || resize) {
        break;
      }
    }
    if (back) {
      strcpy(dir, "..");
    } else if (ext_media) {
      strcpy(dir, ext_media_path);
    } else if (choice != 0) {
      strcpy(dir, namelist[choice - 1]->d_name);
    }
  }
  endwin();

  return 0;
}

// filter that sorts out dot files
int sdfilt(const struct dirent *de) {
  if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
    return 0;
  else
    return 1;
}