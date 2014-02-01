
#include "Camera.h"

const float CAMERA_SPEED = 340.f;
const float ZOOM_SPEED = 1.f;

Camera* Camera::sCurrentCamera = 0;

Camera& Camera::currentCamera() {
	return *sCurrentCamera;
}

Camera::Camera(sf::Window* window) 
: mWindow(window)
, mView()
, mZoom(1.5f)
, mFollowTargetPosition()
, mBounds()
{
	sCurrentCamera = this;
}

// Helper function for camera update
sf::Vector2f Lerp(sf::Vector2f from, sf::Vector2f to, float t )
{
	return from + (to - from) * t;
}

void Camera::update(sf::Time dt) {
	/*
	sf::Vector2f camMovement;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        camMovement.y -= CAMERA_SPEED;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        camMovement.y += CAMERA_SPEED;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        camMovement.x -= CAMERA_SPEED;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        camMovement.x += CAMERA_SPEED;

	move(camMovement * dt.asSeconds());
	*/


	// Smooth follow after target position
	// low damping = slower follow
    float damping = 4.0f;
	setPosition(Lerp(getPosition(), mFollowTargetPosition, dt.asSeconds() * damping));

	// Applies the position to the view
	sf::Vector2f viewSize = sf::Vector2f(float(mWindow->getSize().x),float(mWindow->getSize().y));
	mView = sf::View(getPosition(), viewSize);

	// Zooms in and out withthe pageup and pqgedown keys
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp))
        mZoom -= ZOOM_SPEED * dt.asSeconds();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown))
        mZoom += ZOOM_SPEED * dt.asSeconds(); 
	mView.zoom(mZoom);

	// Size after zoom
	viewSize = mView.getSize();

	// Limit the cameraview to the level bounds
	if (mBounds.width != 0 && mBounds.height != 0)
	{
		float x = getPosition().x;
		float y = getPosition().y;
		// Left boundry
		if ((getPosition().x - viewSize.x/2) < mBounds.left)
			x = mBounds.left + viewSize.x/2;
		// Right boundry
		if ((getPosition().x + viewSize.x/2) > (mBounds.left+mBounds.width))
			x = (mBounds.left+mBounds.width) - viewSize.x/2;
		// Top boundry
		if ((getPosition().y - viewSize.y/2) < mBounds.top)
			y = mBounds.top + viewSize.y/2;
		// Bottom boundry
		if ((getPosition().y + viewSize.y/2) > (mBounds.top+mBounds.height))
			y = (mBounds.top+mBounds.height) - viewSize.y/2;


		setPosition(x, y);
	}
	
	mView.setCenter(getPosition());

	//setPosition(mBounds.left + viewSize.x/2, getPosition().y);
	

	// Sets the position and view size
	
	

	
}



sf::Vector2f Camera::getMousePosition() {
	sf::Vector2f pos;
	pos.x = sf::Mouse::getPosition(*mWindow).x*mZoom + mView.getCenter().x - mView.getSize().x/2;
	pos.y = sf::Mouse::getPosition(*mWindow).y*mZoom + mView.getCenter().y - mView.getSize().y/2;

	return pos;
}

sf::View Camera::getView() {
	return mView;
}

void Camera::setTargetPosition(sf::Vector2f position)
{
	mFollowTargetPosition = position;
}

void Camera::setBounds(sf::FloatRect bounds)
{
	mBounds = bounds;
}