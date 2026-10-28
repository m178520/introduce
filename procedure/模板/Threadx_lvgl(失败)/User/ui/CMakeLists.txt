SET(SOURCES components/ui_comp_button3.c
    components/ui_comp.c
    screens/ui_Screen1.c
    screens/ui_Screen2.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_bk1_png.c
    images/ui_img_computer_png.c
    images/ui_img_bk_png.c
    fonts/ui_font_myfont.c)

add_library(ui ${SOURCES})
