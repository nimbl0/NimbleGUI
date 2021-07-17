#include "BaseElement.h"
#include "Input.h"

Font Gui::textFont;
std::vector<Gui::ElementBase*> Gui::elements;

Gui::ElementBase::ElementBase(void) {
    dimension = Vector2{50, 50};
    pos = Vector2{0, 0};

    color = BLACK;
    visible = true;
    type = ElementType::BASE;
}

Gui::TitleBar::TitleBar(int x, int y, int width) : ElementBase() {
    height = 20;
    pos = Vector2{(float) x, (float) y};
    dimension = Vector2{(float) width, (float) height};
    type = ElementType::TITLEBAR;
}

void Gui::TitleBar::update(void) {
    if(visible) {
        DrawRectangle((int) pos.x, (int) (pos.y - dimension.y), (int) dimension.x, (int) dimension.y, color);
    }
}

Gui::Panel::Panel(int x, int y, int width, int height) : ElementBase() {
    pos = Vector2{(float) x, (float) y};
    dimension = Vector2{(float) width, (float) height};
    hasBorders = false;
    borderColor = WHITE;
    type = ElementType::PANEL;
    titleBar = new TitleBar((int) pos.x, (int) pos.y, (int) dimension.x);
}

Gui::Panel::Panel(int x, int y) : Panel(x, y, 0, 0){
}

Gui::Panel::Panel(Vector2 pos) : Panel((int) pos.x, (int) pos.y){

}

void Gui::Panel::addElement(ElementBase *element) {
    float elementX = element->pos.x;
    float elementY = element->pos.y;
    float panelWidth = dimension.x;
    float panelHeight = dimension.y;

    // Check if given element is in the boundaries of the Panel
    if(elementX > panelWidth || elementX < 0 || elementY > panelHeight || elementY < 0) {
        // TODO add some kind of error message
        return;
    }


    element->pos.x += pos.x;
    element->pos.y += pos.y;

    if(element->type == ElementType::BUTTON) {
        auto* button = dynamic_cast<Button*>(element);
        button->text.pos.x += pos.x;
        button->text.pos.y += pos.y;
    }

    elements.push_back(element);
}

void Gui::Panel::update(void) {
    if(visible) {
        DrawRectangle((int) pos.x, (int) pos.y, (int) dimension.x, (int) dimension.y, color);

        for(auto &element : elements) {
            element->update();
        }

        titleBar->update();
    }
}

Gui::Label::Label(const char* text) : Label(0, 0, text){
}

Gui::Label::Label(float x, float y, const char *text) : ElementBase() {
    pos = Vector2{x, y};
    textSize = 20;
    type = ElementType::LABEL;
    this->text = text;
}

Gui::Label::Label(Vector2 pos, const char* text) : Label(pos.x, pos.y, text){

}

void Gui::Label::update(void) {
    if(visible) {
        DrawTextEx(textFont, text, pos, (float) textSize, 1, color);
    }
}

Gui::Button::Button(Vector2 pos, Vector2 dimension) : ElementBase() {
    type = ElementType::BUTTON;
    text = Label(pos, "");
    this->pos = pos;
    this->dimension = dimension;
    this->offset = pos;
}

void Gui::Button::update(void) {
    if(visible) {
        DrawRectangle((int) pos.x, (int) pos.y, (int) dimension.x, (int) dimension.y, color);
        text.update();
    }
}

void Gui::Button::addAction(ClickAction clickAction) {
    action = clickAction;
}

Gui::Window::Window(float width, float height, const char* title) {
    dimension = Vector2{width, height};
    pos = Vector2{0, 0};
    type = ElementType::WINDOW;
    this->title = title;
    hasBorders = false;
    borderColor = BLACK;
    color = DARKGRAY;

    InitWindow((int) dimension.x, (int) dimension.y, title);
    SetTargetFPS(25);
}

Gui::Window::Window(Vector2 pos, const char* title) : Window(pos.x, pos.y, title) {

}

void Gui::Window::addElement(ElementBase* element) {
    element->pos.x += pos.x;
    element->pos.y += pos.y;
    elements.push_back(element);
}

void Gui::Window::updateElements(void) {
    for(auto &element : elements) {
        element->update();
    }
}

void Gui::Window::update(void) {
    ClearBackground(color);
    updateElements();
}

void Gui::Window::close(void) {
    CloseWindow();
}


/* ------------------------------------------------ */
void Gui::initialise(void) {
    textFont = LoadFont("resources/fonts/Monoid.ttf"); // TODO load font from some kind of config?
}

Gui::Button* Gui::createButton(int x, int y, int width, int height) {
    auto* button = new Button(Vector2{(float) x, (float) y}, Vector2{(float) width, (float) height});
    elements.push_back(button);
    Input::registr(*button);
    return button;
}

Gui::Panel* Gui::createPanel(int x, int y, int width, int height) {
    auto* panel = new Panel(x, y, width, height);
    elements.push_back(panel);
    Input::registr(*panel);
    return panel;
}

Gui::Window* Gui::createWindow(int width, int height, const char* title) {
    auto* window = new Window((float) width, (float) height, title);
    // elements.push_back(window);
    Input::registerContainer(window);
    return window;
}

void Gui::renderElements(void) {
    /*
    for(auto &element : elements) {

    }
    */
}

void Gui::cleanUp(void) {
    for(auto &element : elements) {
        free(element);
    }
}

bool Gui::checkColor(Color first, Color second) {
    unsigned char oldR = first.r;
    unsigned char oldG = first.g;
    unsigned char oldB = first.b;

    unsigned char newR = second.r;
    unsigned char newG = second.g;
    unsigned char newB = second.b;

    return (oldR == newR) && (oldG == newG) && (oldB == newB);
}

Gui::ElementBase* Gui::getElementByPos(Vector2 pos) {
    for(auto &element : elements) {
        // printf("Element: {%f, %f, %f, %f}, MousePos: {%f, %f}\n", element->pos.x, element->pos.y, element->dimension.x, element->dimension.y, pos.x, pos.y);
        bool inX = (pos.x <= element->pos.x + element->dimension.x) && (pos.x >= element->pos.x);
        bool inY = (pos.y <= element->pos.y + element->dimension.y) && (pos.y >= element->pos.y);

        if(inX && inY) {
            return element;
        }
    }
    return nullptr;
}