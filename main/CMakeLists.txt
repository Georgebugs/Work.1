cmake_minimum_required(VERSION 3.16)

idf_component_register(
                    SRCS
                    	main.c
                    	board_GPIOext.c
                    	board_i2c.c
                    	communication._c
                    	display.c
                    	gui.c
                    	ui/actions.c
                    	ui/images.c
                    	ui/screens.c
                    	ui/styles.c
                    	ui/ui.c
                    	ui/vars.c
                    	ui/ui_font_main_rus_14.c
                    	ui/ui_font_main_rus_36.c
                    	ui/ui_image_splash_metran_logo.c
                    	mainboardcommands.c
                    	communication.c
                    	PressureUnits.c
                    PRIV_INCLUDE_DIRS ui)
set_source_files_properties(communication._c PROPERTIES HEADER_FILE_ONLY TRUE)

