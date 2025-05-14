
#include <stdio.h>
#include "../maxssau/maxssau.h"


using namespace maxssau;

int main(int arg_count, char* arg_value[])
{
    XmlHandler xml_writer;

    xml_writer.create("Settings");
    auto writer_root=xml_writer.getRoot();
    auto update=writer_root.addChild("UpdateServer");
    update.addChild("Server","localhost");
    update.addChild("user","John Doe");
    update.addChild("password","dummy");
    xml_writer.save("xml_settings.xml");

    XmlHandler xml_reader;
    xml_reader.open("xml_settings.xml");
    auto reader_root=xml_reader.getRoot();

    for(int i=0;i<reader_root.getChildCount();i++)
    {
        auto node=reader_root.getChild(i);
        printf("child: %s=%s\n",node.getChild(0).getText(),node.getChild(1).getText());
    }
    return 0;
}