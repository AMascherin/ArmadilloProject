#include "ArmadilloGame.h"

using namespace SampleRenderer;
using namespace SampleFramework;

// Funzione per stampare a video il messaggio di fine gioco
void ArmadilloGame::printConclusionMessage() {
	const PxReal scale = 0.6f;
	const PxReal shadowOffset = 6.0f;
	const RendererColor textColor(255, 255, 255, 255);
	const PxU32 yInc = 20;
	
    PxU32	y_coord = getCamera().getScreenHeight() - 18 * 10;
	PxU32   x_coord = 80;
	Renderer* renderer = getRenderer();

	x_coord = getCamera().getScreenWidth() / 2 - (int)floorl(strlen("Ci sei riuscito! Hai trovarto la chiave dell'armadillo!")*scale * 8.5);
	renderer->print(x_coord, y_coord += yInc, "Ci sei riuscito! Hai trovarto la chiave dell'armadillo!", scale, shadowOffset, textColor);

	x_coord = getCamera().getScreenWidth() / 2 - (int)floorl(strlen("Premi Y per tornare al menu principale")*0.8 * 9);
	renderer->print(x_coord, y_coord += (yInc + 10), "Premi Y per tornare al menu principale", 0.8, shadowOffset, textColor);

	x_coord = getCamera().getScreenWidth() / 2 - (int)floorl(strlen("Premi ESC per uscire dal gioco")*0.8 * 9);
	renderer->print(x_coord, y_coord += (yInc + 15), "Premi ESC per uscire dal gioco", 0.8, shadowOffset, textColor);
}

// Funzione per stampare a video il messaggio introduttivo
void ArmadilloGame::printIntroMessage() {
	const PxReal scale = 0.6f;
	const PxReal shadowOffset = 6.0f;
	const RendererColor textColor(255, 255, 255, 255);
	const PxU32 yInc = 20;

	PxU32	y_coord = getCamera().getScreenHeight() - 18 * 10;
	PxU32   x_coord = 80;
	Renderer* renderer = getRenderer();
	
	x_coord = getCamera().getScreenWidth() / 2 - (int)floorl(strlen("In una discarica, un armadillo apre gli occhi ")*scale * 9);
	renderer->print(x_coord, y_coord += yInc, "In una discarica, un armadillo apre gli occhi ", scale, shadowOffset, textColor);

	x_coord = getCamera().getScreenWidth() / 2 - (int)floorl(strlen("Appena sveglio si rese conto che il suo corpo era bloccato.")*scale * 9);
	renderer->print(x_coord, y_coord += yInc, "Appena sveglio si rese conto che il suo corpo era bloccato", scale, shadowOffset, textColor);

	x_coord = getCamera().getScreenWidth() / 2 - (int)floorl(strlen("La sua chiave era scomparsa!")*scale * 9);
	renderer->print(x_coord, y_coord += yInc, "La sua chiave era scomparsa!", scale, shadowOffset, textColor);

	x_coord = getCamera().getScreenWidth() / 2 - (int)floorl(strlen("Ma il suo istinto gli suggeriva di cercare in quella stessa discarica...")*scale * 8.5);
	renderer->print(x_coord, y_coord += yInc, "Ma il suo istinto gli suggeriva di cercare in quella stessa discarica...", scale, shadowOffset, textColor);

	x_coord = getCamera().getScreenWidth() / 2 - (int)floorl(strlen("Premi Y per continuare")*0.8 * 9);
	renderer->print(x_coord, y_coord += (yInc + 10), "Premi Y per continuare", 0.8, shadowOffset, textColor);
}

// Funzione per stampare a video il messaggio di aiuto
void ArmadilloGame::helpRender(PxU32 x, PxU32 y, PxU8 textAlpha)
{
	Renderer* renderer = getRenderer();
	PxU32 yInc = 15;
	PxReal scale = 0.4f;
	const PxReal shadowOffset = 6.0f;
	const RendererColor textColor(255, 255, 255, textAlpha);

	const bool isMouseSupported = getApplication().getPlatform()->getSampleUserInput()->mouseSupported();
	const bool isPadSupported = getApplication().getPlatform()->getSampleUserInput()->gamepadSupported();
	const char* msg;
	
    // Per stampare più "in alto" il messaggio
    y -= 10;

	if (isMouseSupported && isPadSupported) {
		renderer->print(x, y += yInc, "Use mouse or right stick to rotate the camera", scale, shadowOffset, textColor);
		renderer->print(x, y += yInc, "Use the mouse or the right D-Pad to rotate the camera", scale, shadowOffset, textColor);
		renderer->print(x, y += yInc, "Use the spacebar or the A button to jump", scale, shadowOffset, textColor);

	}
	else if (isMouseSupported) {
		renderer->print(x, y += yInc, "Use arrow keys to move", scale, shadowOffset, textColor);
		renderer->print(x, y += yInc, "Use the mouse to rotate the camera", scale, shadowOffset, textColor);
		renderer->print(x, y += yInc, "Use the spacebar to jump", scale, shadowOffset, textColor);

	}
	else if (isPadSupported) {
		renderer->print(x, y += yInc, "Use left D-Pad to move", scale, shadowOffset, textColor);
		renderer->print(x, y += yInc, "Use right D-Pad to rotate the camera", scale, shadowOffset, textColor);
		renderer->print(x, y += yInc, "Use the A button to jump", scale, shadowOffset, textColor);
	}

	msg = mApplication.inputInfoMsg("Press ", " to open the menu", MENU_SELECT, -1);
	renderer->print(x, y += (yInc + 10), msg, scale, shadowOffset, textColor);
	renderer->print(x, y += yInc, "Press ESC to exit the game", scale, shadowOffset, textColor);
	renderer->print(x, y += yInc, "Lo scopo del gioco e' raggiungere la chiave per sbloccare l'armadillo", scale, shadowOffset, textColor);
	renderer->print(x, y += yInc, "Completa il labirinto e raggiungi la fine del percorso", scale, shadowOffset, textColor);

}

// Funzione per stampare a video il messaggio di descrizione del gioco
void ArmadilloGame::descriptionRender(PxU32 x, PxU32 y, PxU8 textAlpha)
{
	bool print = (textAlpha != 0.0f);
	if (print)
	{
		Renderer* renderer = getRenderer();
		PxU32 yInc = 18;
		PxReal scale = 0.5f;
		const PxReal shadowOffset = 0;
		const RendererColor textColor(255, 255, 255, textAlpha);

		if (getCamera().getScreenWidth() <= 800) {
			scale = 0.3f;
			yInc = 12;
		}

		char line1[256] = "Applicazione per il progetto di Informatica Grafica basata su NVIDIA PhysX 3.4";
		char line2[256] = "Autori: Maria Giulia Canu, Alessandro Mascherin";
		char line3[256] = "Il gioco presenti alcuni degli elementi fondamentali delle capacita' di simulazione fisica di PhysX, quali: ";
		char line4[256] = "gestione delle collisioni, applicazione di forze e attriti ad oggetti dinamici, Joint, Kinematic object";
		
        renderer->print(x, y += yInc, line1, scale, shadowOffset, textColor);
		renderer->print(x, y += yInc, line2, scale, shadowOffset, textColor);
		renderer->print(x, y += yInc, line3, scale, shadowOffset, textColor);
		renderer->print(x, y += yInc, line4, scale, shadowOffset, textColor);

	}
}