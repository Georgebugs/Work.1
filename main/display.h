#ifndef MAIN_DISPLAY_H_
#define MAIN_DISPLAY_H_

esp_err_t eDisplayInitLCD(void);
esp_err_t sDisplayInitTouch(void);
esp_err_t eDisplayInitLVGL(void);
esp_err_t eDisplayControlBacklight(const bool bEnable);

#endif /* MAIN_DISPLAY_H_ */
