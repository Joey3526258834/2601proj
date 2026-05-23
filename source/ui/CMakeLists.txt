SET(SOURCES screens/ui_Screen1.c
    screens/ui_Screen2.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    fonts/ui_font_back.c
    fonts/ui_font_eyes.c
    fonts/ui_font_hum.c
    fonts/ui_font_smoke.c
    fonts/ui_font_sun.c
    fonts/ui_font_temp.c
    fonts/ui_font_text.c)

add_library(ui ${SOURCES})
