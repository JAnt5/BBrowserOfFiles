#include <curses.h>
#include <dirent.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <term.h>
#include <unistd.h>

#define MAX_FILE_NAME 255

void file_menu(WINDOW *menu_win, int highlight, int x, int y, int n_choices,
               char *choices[], int isFolder[]) {
  int i;
  x += 2;
  y += 2;

  box(menu_win, 0, 0);
  for (i = 0; i < n_choices; ++i) {
    if (isFolder[i]) {
      wattron(menu_win, COLOR_PAIR(1));
    }
    if (highlight == i + 1) /* High light the present choice */
    {
      wattron(menu_win, A_REVERSE);
      mvwprintw(menu_win, y, x, "%s", choices[i]);

      wattroff(menu_win, A_REVERSE);
    } else
      mvwprintw(menu_win, y, x, "%s", choices[i]);

    if (isFolder[i]) {
      wattroff(menu_win, COLOR_PAIR(1));
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

  struct dirent *de;
  DIR *dr;

  int x, y, dir_entry_count, highlight = 1, choice = 0, c;
  bool back = false, ext_media = false, resize = false;
  while (1) {
    
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

    dr = opendir(dir);
    if (dr == NULL) {
      mvwprintw(window, y, x + 50, "Could not open current directory");
      wrefresh(window);
      return 0;
    }

    dir_entry_count = 0;
    while ((de = readdir(dr)) != NULL) {
      dir_entry_count++;
    }
    // za ponovni read

    rewinddir(dr);

    // Dynamic memory allocation and array
    char **dir_entry = NULL;
    int folder_indeces[dir_entry_count];
    dir_entry = (char **)malloc(dir_entry_count * sizeof(char *));
    for (int i = 0; i < dir_entry_count; i++) {
      dir_entry[i] = (char *)malloc((MAX_FILE_NAME + 1) * sizeof(char));
      if (dir_entry[i] == NULL) {
        mvwprintw(window, y, x + 50, "Could not allocate memory");
        wrefresh(window);

        return 0;
      }
      // Making a lsit
      if ((de = readdir(dr)) != NULL) {
        strcpy(dir_entry[i], de->d_name);
        if (de->d_type == DT_DIR) {
          folder_indeces[i] = 1;
        } else {
          folder_indeces[i] = 0;
        }
      }
    }
    file_menu(window, highlight, x, y, dir_entry_count, dir_entry,
              folder_indeces);
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
      file_menu(window, highlight, x, y, dir_entry_count, dir_entry,
                folder_indeces);
      if (choice != 0 || back || ext_media || resize) {
        break;
      }
    }
    if (back) {
      strcpy(dir, "..");
      back = false;
    } else if (ext_media) {
      strcpy(dir, ext_media_path);
      ext_media = false;
    } else if(choice != 0){
      strcpy(dir, dir_entry[choice - 1]);
    }else if(resize){
      resize = false;
    }

    // freeing memory
    for (int i = 0; i < dir_entry_count; i++) {
      free(dir_entry[i]);
    }
    // Free memory for the array of pointers
    free(dir_entry);
    closedir(dr);
  }
  endwin();

  return 0;
}