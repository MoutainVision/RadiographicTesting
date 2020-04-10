#include "readdcmfile.h"

ReadDCMFile::ReadDCMFile()
{

}

bool ReadDCMFile::readDCMFile(std::string filePath, string outFileName, string errorStr)
{
     DcmFileFormat fileformat;
//     OFCondition oc = fileformat.loadFile("G:\\DCM\\CR\\CR15-020-T10厚-450焦距6分50秒.DCM");
     OFCondition oc = fileformat.loadFile(OFFilename(filePath.c_str()));

     if(oc.good())
     {
         DcmDataset *dataset = fileformat.getDataset();

         unsigned short m_width;		//获取图像的窗宽高
         unsigned short m_height;
         dataset->findAndGetUint16(DCM_Rows, m_height);
         dataset->findAndGetUint16(DCM_Columns, m_width);

         std::cout << "width :" << m_width << std::endl;
         std::cout << "height " << m_height << std::endl;


         DcmElement* element = nullptr;    //读取dcm中的像素值
         OFCondition result = dataset->findAndGetElement(DCM_PixelData, element);
         if (result.bad() || element == nullptr)
         {
             std::cout << "findAndGetElement result  bad. " << std::endl;

             errorStr = "findAndGetElement result  bad. ";
             return false;
         }

         Uint16* pixData16;
         result = element->getUint16Array(pixData16);
         if (result.bad())
         {
             std::cout << "getUint16Array result  bad. " << std::endl;
             errorStr = "getUint16Array result  bad. ";
             return false;
         }

         std::cout << outFileName << std::endl;


		 Uint16 min = 65535, max = 0;
		 for (int y = 0; y < m_height; y++)
			 for (int x = 0; x < m_width; x++)
			 {
				 if (pixData16[y*m_width + x] > max)
				 {
					 max = pixData16[y*m_width + x];
				 }

				 if (pixData16[y*m_width + x] < min)
				 {
					 min = pixData16[y*m_width + x];
				 }
			 }

		 std::cout << min << "\t" << max << std::endl;

		 CxImage img;
		 img.Create(m_width, m_height, 8);
		 RGBQUAD rgb[256];
		 for (int k = 0; k < 256; k++)
		 {
			 rgb[k].rgbBlue = rgb[k].rgbGreen = rgb[k].rgbRed = k;
		 }

		 img.SetPalette(rgb, 256);
		 if (img.IsValid())
		 {
			 std::cout << img.GetWidth() << std::endl;
			 std::cout << img.GetHeight() << std::endl;
			 std::cout << img.GetEffWidth() << std::endl;
			 std::cout << img.GetSize() << std::endl;

			 unsigned char *pimg = img.GetBits();
			 int p = img.GetEffWidth();
			 for (int y = 0; y < m_height; y++)
				 for (int x = 0; x < m_width; x++)
				 {
					 int g = (int)((double)(pixData16[y*m_width + x] - min) / (max - min)*255.0);
					 if (g > 255)
					 {
						 std::cout << g << std::endl;
					 }

					 pimg[y*p + x] = g;
					 //img.SetPixelColor(x, y, RGB(g, g, g));
				 }

			 img.Flip();
			 img.Save(outFileName.c_str(), CXIMAGE_FORMAT_JPG);

			 //const TCHAR * encodedName = reinterpret_cast<const TCHAR *>(outFileName.c_str());
			 
			 //img.Save(encodedName, CXIMAGE_FORMAT_JPG);


		 }


//		 memset(pixData16, 0, element->getLength() / 2);

         //save
         //CxImage *img = new CxImage();
         //img->create(m_width, m_height, true, PIXEL_FORMAT_MONO8);
         //img->copyData((const unsigned char *)(pixData16), img->getImageSize());
         //bool isSuc = img->save(outFileName.c_str());

         //if (!isSuc)
         //    return false;

     }
     else {
         return false;
     }

     return true;
}
