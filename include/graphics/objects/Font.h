#ifndef FONT_H
#define FONT_H

class Font {
   public:
    std::string name;
    std::string style;
    Color color;
    int weigth;
    int size;

    Font(/* args */);
    ~Font();
};

#endif  //FONT_H