#include "QGL.h"
#include <iostream>

using namespace QGL;

// 示例：创建一个简单的自定义控件
class MyButton : public Widget {
    QGL_OBJECT(MyButton)
    
public:
    MyButton(Widget* parent = nullptr) : Widget(parent) {
        setText("My Button");
        setSize(Size(120, 40));
        setBackgroundColor(Color("#3498db"));
        
        // 连接点击事件
        clicked.connect([this]() {
            onButtonClicked();
        });
    }
    
    void setText(const String& text) {
        setProperty("text", Variant::fromString(text));
    }
    
    String getText() const {
        return getProperty("text").toString();
    }
    
    // 信号
    SIGNAL(clicked);
    
protected:
    void mouseClickEvent(const Point& pos) override {
        std::cout << "Button clicked at (" << pos.x << ", " << pos.y << ")" << std::endl;
        clicked.emit();
    }
    
private:
    void onButtonClicked() {
        std::cout << "Button '" << getText() << "' was clicked!" << std::endl;
        
        // 改变颜色作为反馈
        static bool toggle = false;
        toggle = !toggle;
        setBackgroundColor(toggle ? Color("#e74c3c") : Color("#3498db"));
    }
};

// 主函数示例
int main() {
    std::cout << "=== QGL PDK 框架演示 ===" << std::endl;
    std::cout << QGL::getVersionString() << std::endl;
    std::cout << std::endl;
    
    // 初始化框架
    QGL::initialize();
    
    // 创建主窗口
    auto mainWindow = std::make_shared<Widget>();
    mainWindow->setObjectName("MainWindow");
    mainWindow->setSize(Size(800, 600));
    mainWindow->setBackgroundColor(Color("#ecf0f1"));
    
    // 创建按钮
    auto button1 = std::make_shared<MyButton>(mainWindow.get());
    button1->setObjectName("Button1");
    button1->setPosition(Point(50, 50));
    button1->setText("点击我 1");
    
    auto button2 = std::make_shared<MyButton>(mainWindow.get());
    button2->setObjectName("Button2");
    button2->setPosition(Point(200, 50));
    button2->setText("点击我 2");
    button2->setBackgroundColor(Color("#2ecc71"));
    
    // 演示属性系统
    std::cout << "=== 属性系统演示 ===" << std::endl;
    std::cout << "Button1 name: " << button1->getObjectName() << std::endl;
    std::cout << "Button1 text: " << button1->getText() << std::endl;
    std::cout << "Button1 position: " << button1->getPosition().x << ", " << button1->getPosition().y << std::endl;
    std::cout << "Button1 size: " << button1->getSize().width << "x" << button1->getSize().height << std::endl;
    std::cout << std::endl;
    
    // 演示信号槽系统
    std::cout << "=== 信号槽系统演示 ===" << std::endl;
    
    // 连接按钮信号到 lambda 函数
    button1->clicked.connect([&]() {
        std::cout << "Lambda: Button1 clicked!" << std::endl;
        button2->setText("Button1 was clicked!");
    });
    
    button2->clicked.connect([&]() {
        std::cout << "Lambda: Button2 clicked!" << std::endl;
        button1->setText("Button2 was clicked!");
    });
    
    // 模拟点击事件
    std::cout << "模拟点击 Button1..." << std::endl;
    button1->clicked.emit();
    std::cout << std::endl;
    
    std::cout << "模拟点击 Button2..." << std::endl;
    button2->clicked.emit();
    std::cout << std::endl;
    
    // 演示对象查找
    std::cout << "=== 对象查找演示 ===" << std::endl;
    auto foundButton = mainWindow->findChild<MyButton>("Button1");
    if (foundButton) {
        std::cout << "找到按钮: " << foundButton->getObjectName() << std::endl;
        std::cout << "按钮文本: " << foundButton->getText() << std::endl;
    }
    std::cout << std::endl;
    
    // 演示属性变化监听
    std::cout << "=== 属性变化监听演示 ===" << std::endl;
    button1->propertyChanged.connect([](const String& name, const Variant& value) {
        std::cout << "Property changed: " << name << " = " << value.toString() << std::endl;
    });
    
    // 改变属性触发信号
    button1->setPosition(Point(100, 100));
    button1->setBackgroundColor(Color("#9b59b6"));
    std::cout << std::endl;
    
    // 清理框架
    QGL::cleanup();
    
    std::cout << "=== 演示完成 ===" << std::endl;
    return 0;
}