#ifndef COLORS_HPP
    #define COLORS_HPP

    #define RGB(r, g, b) ((r) * 0x10000 + (g) * 0x100 + (b))
    
    struct ColorMap {
        int walls, ground;
        int plant1, plant2;
        int stones;
        int font;
    };

    const ColorMap DEFAULT_COLORS = {13530947, -1, 6270751, 6270751, 11562056, 16711680};

    enum Color {
        RED = RGB(255, 0, 0),
        GREEN = RGB(0, 255, 0),
        BLUE = RGB(0, 0, 255),
        BLACK = RGB(0, 0, 0),
        WHITE = RGB(255, 255, 255),
        STONE = 5528941,
        LIGHT_BLOOD = 16711680
    };
#endif