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
		float m_LocationX;
		float m_LocationY;
		float m_Width;
		float m_Height;
		int m_Progress;
		uint32_t m_Color;
};
