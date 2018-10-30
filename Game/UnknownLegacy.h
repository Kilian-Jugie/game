#pragma once
#include "Renderer.h"

namespace UL {

	using namespace Math::Literals;

	class UnknownLegacy : public Platform::Application {

	public:
		explicit UnknownLegacy(const Arguments& arguments);

	private:
		void drawEvent() override;
		void viewportEvent(ViewportEvent& event) override;
		void mousePressEvent(MouseEvent& event) override;
		void mouseReleaseEvent(MouseEvent& event) override;
		void mouseMoveEvent(MouseMoveEvent& event) override;
		void mouseScrollEvent(MouseScrollEvent& event) override;

		Vector3 positionOnSphere(const Vector2i& position) const;

		UL::ulRenderer m_Renderer;
		Vector3 _previousPosition;
	};

	void UnknownLegacy::drawEvent() {
		m_Renderer.drawEvent();
		swapBuffers();
	}

	void UnknownLegacy::viewportEvent(ViewportEvent& event) {
		m_Renderer.viewportEvent(event.framebufferSize(), event.windowSize());
	}

	void UnknownLegacy::mousePressEvent(MouseEvent& event) {
		if (event.button() == MouseEvent::Button::Left)
			_previousPosition = positionOnSphere(event.position());
	}

	void UnknownLegacy::mouseReleaseEvent(MouseEvent& event) {
		if (event.button() == MouseEvent::Button::Left)
			_previousPosition = Vector3();
	}

	void UnknownLegacy::mouseScrollEvent(MouseScrollEvent& event) {
		if (!event.offset().y()) return;

		m_Renderer.zoom(1.0f - (event.offset().y() > 0 ? 1 / 0.85f : 0.85f));

		redraw();
	}

	Vector3 UnknownLegacy::positionOnSphere(const Vector2i& position) const {
		return m_Renderer.positionOnSphere(position);
	}

	void UnknownLegacy::mouseMoveEvent(MouseMoveEvent& event) {

		if (!(event.buttons() & MouseMoveEvent::Button::Left)) return;

		Vector3 currentPosition = positionOnSphere(event.position());

		//_previousPosition = { _previousPosition.x(), _previousPosition.y(), 1 };
		//currentPosition = { currentPosition.x(), currentPosition.y(), _previousPosition.z() };
		

		

		const Vector3 axis = Math::cross(_previousPosition, currentPosition);
		

		if (_previousPosition.length() < 0.001f || axis.length() < 0.001f) return;

		
		m_Renderer.rotateManupulator(Math::angle(_previousPosition, currentPosition), axis.normalized());
		_previousPosition = currentPosition;

		redraw();
	}

}


//MAGNUM_APPLICATION_MAIN(Magnum::Examples::ViewerExample)