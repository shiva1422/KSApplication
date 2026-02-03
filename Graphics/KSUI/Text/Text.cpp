//
// Created by pspr1 on 10/24/2020.
//

#include <Logger/KSLog.h>
#include "Text.h"

#define LOGTAG "TEXTENGINE"
//TODO REFACTOR

TextEngine *TextEngine::textEngine;

const char* TextEngine::defaultFont="/system/fonts/DroidSans.ttf";
const int TextEngine::defaultFontSize = 32;


void TextEngine::getPixamapFromString(const std::string &pString, PixaMap *pixaMap)
{
    //pixel_size = point_size * resolution / 72//max_advance_Width to quickly calculate advance width of string
    char ch;
    int ymax=0,ymin=0;
    int totalStringWidth=0,stringHeight=0;
    int stringLength = pString.size();
    FT_Error fterror;
    FT_Int32 render_flags = FT_RENDER_MODE_NORMAL;
    FT_Glyph glyphs[stringLength];
    FT_BitmapGlyph bitmaps[stringLength];
    FT_BBox box;
    FT_UInt glyph_index;

    for(int i = 0; i < stringLength ; i++)
    {
        ch = pString[i];

        KSLOGD(LOGTAG, "loadGlyph %c", ch);

        glyph_index = FT_Get_Char_Index(fontFace,ch);

        if(glyph_index == 0)
        {
            KSLOGD(LOGTAG, "error get char index");
        }
        fterror = FT_Load_Glyph(fontFace, glyph_index, FT_LOAD_DEFAULT);
        if (fterror)
        {
            KSLOGE(LOGTAG, "error loading glyph");
            return;
        }

        fterror =FT_Get_Glyph(fontFace->glyph,glyphs+i);// FT_Render_Glyph(fontFace->glyph,FT_RENDER_MODE_NORMAL);//use renderflages oabove
        if (fterror)
        {
            KSLOGE(LOGTAG, "error getting glyph");
            return;
        }

        fterror = FT_Glyph_To_Bitmap(glyphs+i,FT_RENDER_MODE_NORMAL,NULL,0);
        if (fterror)
        {
            KSLOGE(LOGTAG, "errorConverting glyph to bitmap");
            return;
        }

        totalStringWidth+=(glyphs[i]->advance.x>>10)/64;//if used kerning add that distance too
        FT_Glyph_Get_CBox( glyphs[i], FT_GLYPH_BBOX_PIXELS, &box );
        if(box.yMin<ymin)///TO GET THE RIGHT HEIGHT OF THE STRING
        {
            ymin=box.yMin;

        }
        if(box.yMax>ymax)
        {
            ymax=box.yMax;
        }

        KSLOGD(LOGTAG, "the bdfdf is %d,%d,%d", box.xMax, box.xMin, box.yMin);
        bitmaps[i]=(FT_BitmapGlyph)glyphs[i];
    }


    //RENDERING STARTS FROMHERE TO GIVE PIXMAP
    stringHeight=ymax-ymin;
    KSLOGD(LOGTAG, "the height is %d", stringHeight);

    if(pixaMap->pixels != NULL)
    {
        free(pixaMap->pixels);
        pixaMap->pixels = NULL;
    }

    pixaMap->pixels = static_cast<uint8_t *>(malloc(
            totalStringWidth * (stringHeight) * sizeof(uint32_t)));//stringHeight+1 cuz -topbearing in docs
    if(pixaMap->pixels != NULL)
    {
        pixaMap->width=totalStringWidth;pixaMap->height=stringHeight;
        KSLOGE(LOGTAG, "The totla width and height are %d and %d", pixaMap->width, pixaMap->height);
        uint penX = 0, penY = stringHeight+ymin;//penY=stringHeight-1;if(previously not +1)
        unsigned char *pixels = (unsigned char *) pixaMap->pixels;
        memset(pixels, 0, totalStringWidth * stringHeight * sizeof(uint32_t));
        unsigned char *buffer;
        int noRows,pitch,pixmapIndex=0,bitW,bufferIndex=0;
        for (int i = 0; i < stringLength; i++)
        {
            //KSLOGD(TAGLOG,"filling bitmap");
            penX += bitmaps[i]->left;//next bitmap
            penY -= bitmaps[i]->top;
            buffer = bitmaps[i]->bitmap.buffer;//glyphs buffer
            noRows = bitmaps[i]->bitmap.rows;
            bitW = bitmaps[i]->bitmap.width;
            pitch = bitmaps[i]->bitmap.pitch;
            for (int h = 0; h < noRows; h++)
            {
                for (int w = 0; w < bitW; w++)
                {
                    pixmapIndex = (penY + h) * totalStringWidth * 4 + (penX + w) * 4;
                    bufferIndex = (h * pitch) + w;
                    {
                        if( pixels[pixmapIndex + 1]==0)//cuz already filled overlaps chaeck any not only 1;//cuz gy are overlapping improve later with kerning;
                        {
                            pixels[pixmapIndex] = buffer[bufferIndex] ;
                            pixels[pixmapIndex + 1] = buffer[bufferIndex];
                            pixels[pixmapIndex + 2] = buffer[bufferIndex];
                            pixels[pixmapIndex + 3] = buffer[bufferIndex];
                            //textLog("pixel value r %d",pixels[pixmapIndex+3]);
                        }
                    }
                }
            }
            penX += (glyphs[i]->advance.x >> 10) / 64-bitmaps[i]->left;//cuz length-1 coords
            // penX+=bitW;
            penY = stringHeight+ymin;//-1
            FT_Done_Glyph(glyphs[i]);

        }


    }

    KSLOGD(LOGTAG, "got Image FromString");

}

status TextEngine::setFont(const char *fontFile)
{
    FT_Error fterror;
    fterror = FT_New_Face( ftLibrary,fontFile,0,&fontFace );///////see face index why 0;

    if (fterror == FT_Err_Unknown_File_Format )
    {
        KSLOGE(LOGTAG, "FATAL-ERRROR : UNKNOWN FONT FILEFORMAT =>RESET FONT WITH RIGHT FILE FORMAT");
        return -1;//TODO status
    }
    else if(fterror)
    {
        KSLOGE(LOGTAG, "FATAL-ERRROR : ERROR LOADING FONT  =>RESET FONT WITH RIGHT FONT FILE");
        return -1;
    }
    KSLOGD(LOGTAG, "font set successfully glyphCnt %ld", fontFace->num_glyphs);
    return 0;
}
status TextEngine::setTextSize(int width, int height)
{

    KSLOGD(LOGTAG, "%f xdpi %f ydpi", displayParams.xdpi, displayParams.ydpi);
    FT_Error  fterror = FT_Set_Pixel_Sizes(fontFace, 0, height);//FT_Set_Char_Size(fontFace,0*64,height*64,0,0);////
    if(fterror)
    {
        KSLOGE(LOGTAG, "FATAL ERROR : COULD NOT SET font SIZE =>RETRY OR .........");
        return -1;
    }
    fontHeight = height;
    fontWidth = width;
    KSLOGD(LOGTAG, "set font size %d , %d", fontWidth, height);
    return 0;
}
status TextEngine::init()
{
    FT_Error fterror=FT_Init_FreeType(&ftLibrary);
    if(fterror!=0)
    {
        KSLOGE(LOGTAG, "FATAL-ERROR : ERROR INITIALIZING FREETYPE LIBRARY");
        return -1;
    }
    KSLOGI(LOGTAG, "successfully initialized freetype");
    FT_Int major, minor, patch;
    FT_Library_Version(ftLibrary, &major, &minor, &patch);
    KSLOGD(LOGTAG, "FreeType's version is %d.%d.%d\n", major, minor, patch);
    if(setFont(defaultFont) != 0)
        return -1;
    fontWidth = defaultFontSize,fontHeight = defaultFontSize;
    return setTextSize(fontWidth,fontHeight);
}



KSImage *TextEngine::getImageFromText(const std::string &pString) {

    PixaMap pixaMap;

    getPixamapFromString(pString,&pixaMap);

    KSImage *image = nullptr;
    if(pixaMap.pixels != nullptr)
    {
        image = new KSImage;
        image->data = pixaMap.pixels;
        image->width = pixaMap.width;
        image->height = pixaMap.height;
    }

    return image;
}
