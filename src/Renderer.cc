// $Id$

#include "Renderer.hh"
#include "CommandController.hh"
#include "RenderSettings.hh"


/*
TMS99X8A palette.
Source: TMS9918/28/29 Data Book, page 2-17.
Conversion to RGB was done for MESS by R. Nabet.

First 3 columns from TI datasheet (in volts).
Next 3 columns based on formula :
	Y = .299*R + .587*G + .114*B (NTSC)
(the coefficients are likely to be slightly different with PAL, but who cares ?)
I assumed the "zero" for R-Y and B-Y was 0.47V.
Last 3 coeffs are the 8-bit values.

Color            Y  	R-Y 	B-Y 	R   	G   	B   	R	G	B
0 Transparent
1 Black         0.00	0.47	0.47	0.00	0.00	0.00	  0	  0	  0
2 Medium green  0.53	0.07	0.20	0.13	0.79	0.26	 33	200	 66
3 Light green   0.67	0.17	0.27	0.37	0.86	0.47	 94	220	120
4 Dark blue     0.40	0.40	1.00	0.33	0.33	0.93	 84	 85	237
5 Light blue    0.53	0.43	0.93	0.49	0.46	0.99	125	118	252
6 Dark red      0.47	0.83	0.30	0.83	0.32	0.30	212	 82	 77
7 Cyan          0.73	0.00	0.70	0.26	0.92	0.96	 66	235	245
8 Medium red    0.53	0.93	0.27	0.99	0.33	0.33	252	 85	 84
9 Light red     0.67	0.93	0.27	1.13(!)	0.47	0.47	255	121	120
A Dark yellow   0.73	0.57	0.07	0.83	0.76	0.33	212	193	 84
B Light yellow  0.80	0.57	0.17	0.90	0.81	0.50	230	206	128
C Dark green    0.47	0.13	0.23	0.13	0.69	0.23	 33	176	 59
D Magenta       0.53	0.73	0.67	0.79	0.36	0.73	201	 91	186
E Gray          0.80	0.47	0.47	0.80	0.80	0.80	204	204	204
F White         1.00	0.47	0.47	1.00	1.00	1.00	255	255	255
*/
const byte Renderer::TMS99X8A_PALETTE[16][3] = {
	{   0,   0,   0 },
	{   0,   0,   0 },
	{  33, 200,  66 },
	{  94, 220, 120 },
	{  84,  85, 237 },
	{ 125, 118, 252 },
	{ 212,  82,  77 },
	{  66, 235, 245 },
	{ 252,  85,  84 },
	{ 255, 121, 120 },
	{ 212, 193,  84 },
	{ 230, 206, 128 },
	{  33, 176,  59 },
	{ 201,  91, 186 },
	{ 204, 204, 204 },
	{ 255, 255, 255 }
};

/*
Sprite palette in Graphic 7 mode.
See page 98 of the V9938 data book.
*/
const word Renderer::GRAPHIC7_SPRITE_PALETTE[16] = {
	0x000, 0x002, 0x030, 0x032, 0x300, 0x302, 0x330, 0x332,
	0x472, 0x007, 0x070, 0x077, 0x700, 0x707, 0x770, 0x777
};

Renderer::Renderer(bool fullScreen) :
	fullScreenCmd(this)
{
	this->fullScreen = fullScreen;
	settings = RenderSettings::instance();
	CommandController::instance()->registerCommand(&fullScreenCmd, "fullscreen");
}

Renderer::~Renderer()
{
	CommandController::instance()->unregisterCommand(&fullScreenCmd, "fullscreen");
}

// Note: Renderers override setFullScreen.
void Renderer::setFullScreen(bool enabled)
{
	fullScreen = enabled;
}

void Renderer::checkFullScreen()
{
	bool fsUser = settings->getFullScreen()->getValue();
	if (fsUser != fullScreen) setFullScreen(fsUser);
}


// FullScreen command
Renderer::FullScreenCmd::FullScreenCmd(Renderer *rend)
{
	renderer = rend;
}

void Renderer::FullScreenCmd::execute(const std::vector<std::string> &tokens,
                                      const EmuTime &time)
{
	// Change the setting value. Later this change will be detected and
	// the actual full screen toggle of the Renderer will take place.
	BooleanSetting *setting = RenderSettings::instance()->getFullScreen();
	switch (tokens.size()) {
	case 1:
		setting->setValue(!setting->getValue());
		break;
	case 2:
		setting->setValueString(tokens[1]);
		break;
	default:
		throw CommandException("Syntax error: too many parameters");
	}
}
void Renderer::FullScreenCmd::help(const std::vector<std::string> &tokens) const
{
	print("This command turns full-screen display on/off");
	print(" fullscreen:     toggle full-screen");
	print(" fullscreen on:  switch to full-screen display");
	print(" fullscreen off: switch to windowed display");
}
