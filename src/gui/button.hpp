#include "gui.hpp"

class Button {
	public:
		Button(float x, float y, int imageid);
		Button(float x, float y, float w, float h, char* string);
		~Button();
		void SetSize(float w, float h);
		void SetLocation(float x, float y);
		void SetIcon(int imageid);
		bool Update(int clickX, int clickY);
		void Draw(void);
	private:
		float m_LocationX;
		float m_LocationY;
		float m_Width;
		float m_Height;
		int m_imageid;
		char* m_str = NULL;
};
