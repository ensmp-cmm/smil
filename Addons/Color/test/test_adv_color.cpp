

#include "Core/include/DCore.h"
#include "DAdvColor.h"


using namespace smil;


class Test_Conversions : public TestCase
{
    virtual void run()
    {
        Image<RGB> im1(5, 1);
        Image<RGB> im2(im1);
        Image<RGB> im3(im1);
        
        fill(im1, RGB(0));
        
        UINT8 vecRGB[] = { 
          255, 0, 127, 50, 4,
          1, 17, 41, 14, 250,
          25, 10, 27, 150, 45 
        };
        im1 << RGBArray(vecRGB, 5);

        
        // XYZ
        UINT8 vecXYZ[] = { 
          163, 5, 91, 64, 56,
          80, 11, 65, 40, 153,
          24, 10, 28, 142, 56 
        };
        Image<RGB> imXYZ(im1);
        imXYZ << RGBArray(vecXYZ, 5);
        
        RGBToXYZ(im1, im2);
        TEST_ASSERT(im2==imXYZ);
        
        UINT8 vecRGB_back[] = { 
          254, 0, 126, 50, 4,
          1, 16, 41, 13, 249,
          25, 9, 27, 149, 44 
        };
        Image<RGB> imRGB(im1);
        imRGB << RGBArray(vecRGB_back, 5);
        
        XYZToRGB(im2, im3);
        TEST_ASSERT(im3==imRGB);
        
        
        // LAB
        UINT8 vecLAB[] = { 
          160, 63, 147, 119, 209,
          249, 106, 191, 200, 19,
          171, 120, 155, 50, 175 
        };
        Image<RGB> imLAB(im1);
        imLAB << RGBArray(vecLAB, 5);
        
        XYZToLAB(im2, im3);
        TEST_ASSERT(im3==imLAB);
        
        UINT8 vecXYZ_back[] = { 
          162, 5, 91, 64, 56,
          80, 11, 65, 40, 154,
          24, 10, 28, 144, 56 
        };
        imXYZ << RGBArray(vecXYZ_back, 5);
        
        LABToXYZ(im3, im2);
        TEST_ASSERT(im2==imXYZ);
        
        RGBToLAB(im1, im2);
        TEST_ASSERT(im2==imLAB);
        
        UINT8 vecLAB_back[] = { 
          252, 0, 126, 49, 3,
          2, 16, 41, 13, 251,
          25, 9, 27, 151, 44 
        };
        imLAB << RGBArray(vecLAB_back, 5);
        
        LABToRGB(im2, im3);
        TEST_ASSERT(im3==imLAB);
        

        // HLS L1
        UINT8 vecHLS[] = { 
          252, 111,   5, 180,  91,
           94,   9,  65,  71, 100,
          242,  14,  93, 118, 226 
        };
        Image<RGB> imHLS(im1);
        imHLS << RGBArray(vecHLS, 5);
        
        RGBToHLS(im1, im2);
        TEST_ASSERT(im2==imHLS);
        
        UINT8 vecRGB_back2[] = { 
          255, 0, 127, 50, 3,
          0, 17, 41, 13, 251,
          25, 10, 27, 150, 46 
        };
        imRGB << RGBArray(vecRGB_back2, 5);
        
        HLSToRGB(im2, im3);
        TEST_ASSERT(im3==imRGB);
        
//         im1.printSelf(1);
//         im2.printSelf(1);
//         im3.printSelf(1);
    }
};


int main(int argc, char *argv[])
{
      TestSuite ts;
      ADD_TEST(ts, Test_Conversions);

      return ts.run();
  
}
