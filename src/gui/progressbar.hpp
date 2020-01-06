#include "gui.hpp"

class ProgressBar {
	public:
		ProgressBar(float x, float y, float w, float h, uint32_t color);
		~ProgressBar();
		void UpdateColor(uint32_t color);
		void UpdateSize(float x, float y);
		void UpdateLocation(float x, float y);
		void UpdateProgress(int percent);
		int SeekByClick(uint32_t clickX, uint32_t clickY);
		void Draw(void);
	private:
		float locationX;
		float locationY;
		float width;
		float height;
		int progress;
		uint32_t color;
};
