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

         //save
         avs::xImage *img = new avs::xImage();
         img->create(m_width, m_height, true, avs::PIXEL_FORMAT_MONO8);
         img->copyData((const unsigned char *)(pixData16), img->getImageSize());
         bool isSuc = img->save(outFileName.c_str());

         if (!isSuc)
             return false;

     }
     else {
         return false;
     }

     return true;
}
