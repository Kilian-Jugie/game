#pragma once
#include "Renderer.h"

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

		void normalizeVector(Vector3& v); //To move !!!
		template<typename T>int vectorDotProduct(const Magnum::Math::Vector2<T>& v1, const Magnum::Math::Vector2<T>& v2);

		UL::ulRenderer m_Renderer;
		Vector2i m_PreviousPosition;
		int velocity;
	};

	void UnknownLegacy::drawEvent() {
		m_Renderer.drawEvent();
		swapBuffers();
	}

	void UnknownLegacy::tickEvent() {
		//Debug{} << "TICK";
	}

	void UnknownLegacy::keyPressEvent(KeyEvent& event) {
		if (velocity < 20000)
			velocity += 2000;
		Vector3 direction;
		switch (event.key())
		{
		case KeyEvent::Key::Z: direction = { 0.f,0.f,-0.00001f };
			break;
		case KeyEvent::Key::S: direction = { 0.f,0.f,0.00001f };
			break;
		case KeyEvent::Key::Q: direction = { -0.00001f,0.f,0.f };
			break;
		case KeyEvent::Key::D: direction = { .00001f, 0.f, 0.f };
			break;
		case KeyEvent::Key::Esc: 
			std::exit(0);
		}
		for (unsigned int i(velocity); i; --i) { //Camera smoothing system
			m_Renderer.translateCamera(direction);
			redraw();
		}
	}

	void UnknownLegacy::keyReleaseEvent(KeyEvent& event) {
		velocity = 1;
	}

	void UnknownLegacy::viewportEvent(ViewportEvent& event) {
		m_Renderer.viewportEvent(event.framebufferSize(), event.windowSize());
	}

	void UnknownLegacy::mousePressEvent(MouseEvent& event) {
		if (event.button() == MouseEvent::Button::Left)
			m_PreviousPosition = event.position();// positionOnSphere(event.position());
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

	void UnknownLegacy::normalizeVector(Vector3& v) {
		v.x() = v.x() / v.length();
		v.y() = v.y() / v.length();
		v.z() = v.z() / v.length();
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