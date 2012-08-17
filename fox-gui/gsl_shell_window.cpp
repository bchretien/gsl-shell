#include "gsl_shell_window.h"
#include "gsl_shell_app.h"
#include "luajit.h"

#ifdef WIN32
#define CONSOLE_FONT "consolas"
#else
#define CONSOLE_FONT "monospace"
#endif

FXDEFMAP(gsl_shell_window) gsl_shell_window_map[]=
{
    FXMAPFUNC(SEL_CLOSE, 0, gsl_shell_window::on_close),
    FXMAPFUNC(SEL_COMMAND, FXTopWindow::ID_CLOSE, gsl_shell_window::on_close),
    FXMAPFUNC(SEL_CHANGED, gsl_shell_window::ID_CONSOLE, gsl_shell_window::on_change_console),
    FXMAPFUNC(SEL_COMMAND, gsl_shell_window::ID_ABOUT, gsl_shell_window::on_cmd_about),
};

FXIMPLEMENT(gsl_shell_window,FXMainWindow,gsl_shell_window_map,ARRAYNUMBER(gsl_shell_window_map))

gsl_shell_window::gsl_shell_window(gsl_shell_thread* gs, FXApp* app, const FXString& name, FXIcon *ic, FXIcon *mi, FXint w, FXint h):
    FXMainWindow(app, name, ic, mi, DECOR_ALL, 0, 0, w, h)
{
    m_menu_bar = new FXMenuBar(this, LAYOUT_SIDE_TOP|LAYOUT_FILL_X);
    m_status_bar = new FXStatusBar(this, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|FRAME_RAISED|STATUSBAR_WITH_DRAGCORNER);

    m_file_menu = new FXMenuPane(this);
    new FXMenuCommand(m_file_menu, "&Restart\tCtl-R", NULL, app, gsl_shell_app::ID_LUA_RESTART);
    new FXMenuCommand(m_file_menu, "&Quit\tCtl-Q", NULL, this, FXTopWindow::ID_CLOSE);
    new FXMenuTitle(m_menu_bar, "&File", NULL, m_file_menu);

    m_help_menu = new FXMenuPane(this);
    new FXMenuCommand(m_help_menu, "About", NULL, this, ID_ABOUT);
    new FXMenuTitle(m_menu_bar, "&Help", NULL, m_help_menu, LAYOUT_RIGHT);

    FXVerticalFrame* frame = new FXVerticalFrame(this, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y);

    // Sunken border for text widget
    FXHorizontalFrame *textbox = new FXHorizontalFrame(frame, FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);

    m_text_font = new FXFont(app, CONSOLE_FONT, 11);
    m_text = new fx_console(gs, textbox, this, ID_CONSOLE, LAYOUT_FILL_X|LAYOUT_FILL_Y);
    m_text->setFont(m_text_font);
}

void gsl_shell_window::create()
{
    FXMainWindow::create();
    show(PLACEMENT_SCREEN);
}

long gsl_shell_window::on_close(FXObject* obj, FXSelector sel, void* ptr)
{
    FXApp* app = getApp();
    app->handle(this, FXSEL(SEL_COMMAND, gsl_shell_app::ID_CONSOLE_CLOSE), NULL);
    return 0;
}

long gsl_shell_window::on_change_console(FXObject*, FXSelector, void*)
{
    return m_text->update_editable();
}


long
gsl_shell_window::on_cmd_about(FXObject*, FXSelector, void*)
{
  FXDialogBox about(this,"About GSL Shell",DECOR_TITLE|DECOR_BORDER,0,0,0,0, 0,0,0,0, 0,0);
  FXVerticalFrame* side=new FXVerticalFrame(&about,LAYOUT_SIDE_RIGHT|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 10,10,10,10, 0,0);
  new FXLabel(side,"G S L   S h e l l",NULL,JUSTIFY_LEFT|ICON_BEFORE_TEXT|LAYOUT_FILL_X);
  new FXHorizontalSeparator(side,SEPARATOR_LINE|LAYOUT_FILL_X);
  new FXLabel(side, FXStringFormat(
    "GSL Shell %s, Copyright (C) 2009-2012 Francesco Abbate\n"
    "GNU Scientific Library, Copyright (C) The GSL Team\n"
    "%s -- %s\n",
    GSL_SHELL_RELEASE, LUAJIT_VERSION, LUAJIT_COPYRIGHT),
    NULL,JUSTIFY_LEFT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXButton *button=new FXButton(side,"&OK",NULL,&about,FXDialogBox::ID_ACCEPT,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT,0,0,0,0,32,32,2,2);
  button->setFocus();
  about.execute(PLACEMENT_OWNER);
  return 1;
}
