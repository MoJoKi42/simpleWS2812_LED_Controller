// HSV Umwandlung Rot 0-255
inline uint8_t HSV_to_Red(uint16_t hsv) {

  while (hsv > 360) { hsv = hsv - 360; }
	
	if (hsv <= 60 && hsv >= 300)		// Bereich 0-60 und 300-360
	{
		return 255;
	}
	
	
	if (hsv >= 120 && hsv <= 240)		// Bereich 120 bis 240
	{
		return 0;
	}
	
	
	if (hsv > 60 && hsv < 120)			// Bereich 60 bis 120
	{
		float var = 4.25*(hsv-60);
		return 255 - round(var);
	}
	
	
	if (hsv > 240 && hsv < 300)			// Bereich 240 bis 300
	{
		float var = 4.25*(hsv-240);
		return round(var);
	}
	
	
	return 255;
	
}

// HSV Umwandlung Grün 0-255
inline uint8_t HSV_to_Green(uint16_t hsv) {

  while (hsv > 360) { hsv = hsv - 360; }
	
	if (hsv >= 60 && hsv <= 180)		// Bereich 60 bis 180
	{
		return 255;
	}
	
	
	if (hsv >= 240 && hsv <= 360)		// Bereich
	{
		return 0;
	}
	
	
	if (hsv >= 0 && hsv < 60)			// Bereich
	{
		float var = 4.25*hsv;
		return round(var);
	}
	
	
	if (hsv > 180 && hsv < 240)			// Bereich
	{
		float var = 4.25*(hsv-180);
		return 255 - round(var);
	}
	
	
	return 255;
	
}

// HSV Umwandlung Blau 0-255
inline uint8_t HSV_to_Blue(uint16_t hsv) {

  while (hsv > 360) { hsv = hsv - 360; }
	
	if (hsv >= 180 && hsv <= 300)		// Bereich
	{
		return 255;
	}
	
	
	if (hsv >= 0 && hsv <= 120)			// Bereich
	{
		return 0;
	}
	
	
	if (hsv > 120 && hsv < 180)			// Bereich
	{
		float var = 4.25*(hsv-120);
		return round(var);
	}
	
	
	if (hsv > 300 && hsv <= 360)			// Bereich
	{
		float var = 4.25*(hsv-300);
		return 255 - round(var);
	}
	
	
	return 255;
	
}





