#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

int windowWidth = 800;
int windowHeight = 600;

float ballRadius = 10.0;
float ballVelocity = 8.0;

float paddleWidth = 60.0;
float paddleHeight = 20.0;
float paddleVelocity = 6.0;
int paddleInitialY = windowHeight - 50;
int paddleInitialX = 400;

float brickWidth = 60;
float brickHeight = 20;
int brickRows = 4;
int brickColumns = 11;

struct Paddle  //Player Object
{
	sf::RectangleShape shape;
	sf::Vector2f velocity{ 0.f, 0.f };

	Paddle(float mX, float mY) //Constructor creates the object and sets all the default values
	{
		shape.setSize(sf::Vector2f(paddleWidth,paddleHeight));
		shape.setPosition(sf::Vector2f(mX, mY));
		shape.setFillColor(sf::Color::Blue);
		shape.setOrigin(paddleWidth / 2, paddleHeight / 2);
	}

	void Update() //Update gets called every frame in our main game loop
	{
		if ((isPressingLeft() && !isPressingRight()) && left() > 0) velocity.x = -paddleVelocity; //Move left if you press left and you aren't off the window
		else if ((isPressingRight() && !isPressingLeft()) && right() < windowWidth) velocity.x = paddleVelocity; //Move right if you press right and you aren't off the window
		else velocity.x = 0; //otherwise don't move

		shape.move(velocity);
	}

	//Ease of use functions to allow me to quickly and easily get different properties
	bool isPressingLeft() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A); } //Find if you're pressing the button
	bool isPressingRight() { return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D); } //Find if you're pressing the button
	float x() { return shape.getPosition().x; }
	float y() { return shape.getPosition().y; }
	float left() { return shape.getPosition().x - paddleWidth/2; } //Find left side
	float right() { return shape.getPosition().x + paddleWidth/2; } //Find right side
	float top() { return shape.getPosition().y - paddleHeight / 2; } //Find Top side
	float bottom() { return shape.getPosition().y + paddleHeight / 2; } //Find Bottom Side

};

struct Ball //Ball Object
{
	sf::CircleShape shape;

	sf::Vector2f velocity{-ballVelocity,-ballVelocity}; //Stores ball's Speed

	Ball(float mX,float mY) //Ball Constructor
	{
		shape.setPosition(mX, mY);
		shape.setRadius(ballRadius);
		shape.setFillColor(sf::Color::Red);
		shape.setOrigin(ballRadius,ballRadius);
	}

	void Update() //Called every frame in main game loop
	{ 
		if (left() < 0) velocity.x = ballVelocity; //if you go all the way to the left start moving right;
		if (right() > windowWidth) velocity.x = -ballVelocity; //if you go all the way to the right start moving left;
		if (top() < 0) velocity.y = ballVelocity; //If you go all the way up start moving down
		if (bottom() > windowHeight) velocity.y = -ballVelocity; //if you go all the way down start moving up
		shape.move(velocity); 
	}

	//Stores convenience variables so I can quickly access various properties without typing out a long command every time
	float x() { return shape.getPosition().x; }
	float y() { return shape.getPosition().y; }
	float left() { return shape.getPosition().x - ballRadius; }
	float right() { return shape.getPosition().x + ballRadius; }
	float top() { return shape.getPosition().y - ballRadius; }
	float bottom() { return shape.getPosition().y + ballRadius; }

};

struct Brick //Brick Object
{
	sf::RectangleShape shape; //Stores brick's rendering info
	bool destroyed = false; //Stores whether brick has been destroyed yet.

	Brick(float mX, float mY) //Brick Constructor
	{
		shape.setSize(sf::Vector2f(brickWidth, brickHeight));
		shape.setOrigin(brickWidth / 2, brickHeight / 2);
		shape.setPosition(sf::Vector2f(mX, mY));
		shape.setFillColor(sf::Color::Yellow);
	}

	//Convenience functions
	float x() { return shape.getPosition().x; }
	float y() { return shape.getPosition().y; }
	float left() { return shape.getPosition().x - brickWidth / 2; } //Find left side
	float right() { return shape.getPosition().x + brickWidth / 2; } //Find right side
	float top() { return shape.getPosition().y - brickHeight / 2; } //Find Top side
	float bottom() { return shape.getPosition().y + brickHeight / 2; } //Find Bottom Side
};

template <class T1, class T2> bool isIntersecting(T1& mA, T2& mB)
{
	return mA.right() >= mB.left() && mA.left() <= mB.right() && mA.top() <= mB.bottom() && mA.bottom() >= mB.top();
}

void TestCollision(Paddle& player, Ball& ball)
{
	if (!isIntersecting(player, ball)) return; //do nothing if they aren't intersecting

	ball.velocity.y = -ballVelocity; //make the ball go up
	if (ball.x() < player.x()) ball.velocity.x = -ballVelocity; //If it's left of center then bounce left
	else ball.velocity.x = ballVelocity; //otherwise bounce right

}

void TestCollision(Brick& brick, Ball& ball)
{
	if (!isIntersecting(brick,ball)) return; //exit if they aren't touching

	brick.destroyed = true; //otherwise the brick got hit and is therefore destroyed

	float overlapLeft = brick.left() - ball.right();
	float overlapRight = ball.left() - brick.right();
	float overlapTop = brick.top() - ball.bottom();
	float overlapBottom = ball.top() - brick.bottom();

	bool hitFromLeft = abs(overlapLeft) < abs(overlapRight); //Smaller one is the one that hit
	bool hitFromTop = abs(overlapTop) < abs(overlapBottom); //Smaller one is the one that hit

	float minOverlapX = hitFromLeft ? abs(overlapLeft) : abs(overlapRight);
	float minOverlapY = hitFromTop ? abs(overlapTop) : abs(overlapBottom);

	if (minOverlapX < minOverlapY) 
		ball.velocity.x = hitFromLeft ? -ballVelocity : ballVelocity;
	else
		ball.velocity.y = hitFromTop ? -ballVelocity : ballVelocity;
}

int main()
{
	Ball ball(windowWidth / 2, windowHeight / 2); //create the ball;
	Paddle player(paddleInitialX, paddleInitialY); //create the player
	std::vector<Brick> bricks; //create a vector to hold all the bricks
	

	//Fill up the brick vector with 2 FOR statements to fill up the rows and columns.
	for (int iY{0}; iY < brickRows; iY++)
	{
		for (int iX{0}; iX < brickColumns; iX++)
			bricks.emplace_back((iX + 1) * (brickWidth + 3) + 22, (iY + 2) * (brickHeight + 3));
	}

	sf::RenderWindow window( sf::VideoMode(windowWidth, windowHeight,16) , "Arkanoid Clone", sf::Style::None);
	window.setFramerateLimit(60);
	
	while (true)
	{
		window.clear(sf::Color::Black); //Remove last frame and replace it with black screen

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) break; //exit if escape is pressed

		ball.Update(); //Run Update Functions
		player.Update();

		TestCollision(player, ball);
		for (Brick& brick : bricks) if(!brick.destroyed) TestCollision(brick, ball);

		window.draw(player.shape); //Draw Objects
		window.draw(ball.shape); 
		for (Brick& brick : bricks) if(!brick.destroyed) window.draw(brick.shape); //draw ALL the bricks
		
		
		window.display(); //Display all the stuff we've drawn

		sf::Event event; //Check for Events to stop Window from freezing
		window.pollEvent(event);
	}
	
	return 0;
}