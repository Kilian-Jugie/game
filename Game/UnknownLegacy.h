#pragma once
#include "Renderer.h"
#include "RessourceManager.h"

namespace UL {

	using namespace Math::Literals;

	class UnknownLegacy : public Platform::Application {

	public:
		explicit UnknownLegacy(const Arguments& arguments);

	private:
		void drawEvent() override final;
		void viewportEvent(ViewportEvent& event) override final;
		void mousePressEvent(MouseEvent& event) override final;
		void mouseReleaseEvent(MouseEvent& event) override final;
		void mouseMoveEvent(MouseMoveEvent& event) override final;
		void mouseScrollEvent(MouseScrollEvent& event) override final;
		void keyPressEvent(KeyEvent& event) override final;
		void keyReleaseEvent(KeyEvent& event) override final;
		void tickEvent() override final;


		Vector3 positionOnSphere(const Vector2i& position) const;

		template<typename T>int vectorDotProduct(const Magnum::Math::Vector2<T>& v1, const Magnum::Math::Vector2<T>& v2);//To move !!!

		void testDebug() {
			Info{} << "WARNING ! THIS IS A DEVELOPPEMENT VERSION !!!\n";

			RessourceFile f;
			f.parseFileName("C:\\elfichier.obj");
			system("pause");
		}

		UL::ulRenderer m_Renderer;
		Vector2i m_PreviousPosition;
		Vector3 m_Direction;
		int velocity;
	};

	void UnknownLegacy::drawEvent() {
		m_Renderer.drawEvent();
		swapBuffers();
	}

	void UnknownLegacy::tickEvent() {
		if (velocity) {
			if (velocity < 2000)
				velocity += 20;
			m_Renderer.translateCamera(m_Direction*velocity);
			redraw();
		}
	}

	void UnknownLegacy::keyPressEvent(KeyEvent& event) {
		switch (event.key())
		{
		case KeyEvent::Key::Z: m_Direction = { 0.f,0.f,-0.00001f };
							   break;
		case KeyEvent::Key::S: m_Direction = { 0.f,0.f,0.00001f };
							   break;
		case KeyEvent::Key::Q: m_Direction = { -0.00001f,0.f,0.f };
							   break;
		case KeyEvent::Key::D: m_Direction = { .00001f, 0.f, 0.f };
							   break;
		case KeyEvent::Key::Esc:
			std::exit(0);
		}
		if (!velocity) {
			velocity = 1;
		}
	}

	void UnknownLegacy::keyReleaseEvent(KeyEvent& event) {
		velocity = 0;
	}

	void UnknownLegacy::viewportEvent(ViewportEvent& event) {
		m_Renderer.viewportEvent(event.framebufferSize(), event.windowSize());
	}

	void UnknownLegacy::mousePressEvent(MouseEvent& event) {
		if (event.button() == MouseEvent::Button::Left)
			m_PreviousPosition = event.position();
	}

	void UnknownLegacy::mouseReleaseEvent(MouseEvent& event) {
		if (event.button() == MouseEvent::Button::Left)
			m_PreviousPosition = Vector2i();
	}

	void UnknownLegacy::mouseScrollEvent(MouseScrollEvent& event) {
		if (!event.offset().y()) return;

		redraw();
	}

	Vector3 UnknownLegacy::positionOnSphere(const Vector2i& position) const {
		return m_Renderer.positionOnSphere(position);
	}

	template<typename T>
	inline int UL::UnknownLegacy::vectorDotProduct(const Magnum::Math::Vector2<T>& v1, const Magnum::Math::Vector2<T>& v2) {
		return v1.x()*v2.x()+v1.y()*v2.y();
	}

	void UnknownLegacy::mouseMoveEvent(MouseMoveEvent& event) {

		if (!(event.buttons() & MouseMoveEvent::Button::Left)) return;

		Vector2i currentPosition = event.position();

		m_Renderer.rotateCamera(-(Magnum::Rad)(currentPosition.x()-m_PreviousPosition.x())/100, { 0.f,1.f,0.f }); // 100=inverted sensibility
		m_Renderer.rotateCamera(-(Magnum::Rad)(currentPosition.y() - m_PreviousPosition.y()) /100, { 1.f,0.f,0.f });

		m_PreviousPosition = currentPosition;

		redraw();
	}

}