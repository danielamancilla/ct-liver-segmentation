#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageDuplicator.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include <string> 


typedef signed short PixelType;
const unsigned int Dimension = 2;
typedef itk::Image< PixelType, Dimension > ImageType;

/*
**
** CT Liver segmentation with one seed (ConnectedThresholdImageFilter)
**
** D. Mancilla, D. Pedraza
** Pontificia Universidad Javeriana, 2017
**
** e.g. for 000016.dcm image, use:
** seedX : 330
** seedY : 215
** lowerThreshold : 96
** upperThreshold : 160
**
**/

int main(int argc, char* argv[]) {

	if (argc < 7) {
		std::cerr << "\nUsage: " << std::endl;
		std::cerr << argv[0];
		std::cerr << " output seedX seedY lowerThreshold upperThreshold AllInputs...";
		std::cerr << std::endl;
		return EXIT_FAILURE;
	}

	// Initializations

	const char * outputFileName = argv[1];
	std::string fileName(argv[1]);
	const int seedX = atoi(argv[2]);
	const int seedY = atoi(argv[3]);
	const PixelType lowerThreshold = atoi(argv[4]);
	const PixelType upperThreshold = atoi(argv[5]);

	std::string extension = "";
	extension.insert(extension.begin(), fileName.back());
	fileName.pop_back();
	extension.insert(extension.begin(), fileName.back());
	fileName.pop_back();
	extension.insert(extension.begin(), fileName.back());
	fileName.pop_back();
	extension.insert(extension.begin(), fileName.back());
	fileName.pop_back();
	std::cout << extension;

	for (int i = 6; i < argc; i++) {
		try {
			const char * inputFileName = argv[i];
			// Reader for DICOM slice
			typedef itk::ImageFileWriter< ImageType >  WriterType;
			typedef itk::ImageFileReader< ImageType >  ReaderType;
			ReaderType::Pointer reader = ReaderType::New();
			reader->SetFileName(inputFileName);

			typedef itk::GDCMImageIO ImageIOType;
			ImageIOType::Pointer dicomIO = ImageIOType::New();
			reader->SetImageIO(dicomIO);
			reader->Update();
			ImageType::Pointer inputImage = reader->GetOutput();

			// Smooth image
			typedef itk::CurvatureFlowImageFilter< ImageType, ImageType > CurvatureFlowImageFilterType;
			CurvatureFlowImageFilterType::Pointer smoother = CurvatureFlowImageFilterType::New();
			smoother->SetNumberOfIterations(5);
			smoother->SetTimeStep(0.125);
			smoother->SetInput(inputImage);

			// Region grow
			typedef itk::ConnectedThresholdImageFilter< ImageType, ImageType > ConnectedFilterType;
			ConnectedFilterType::Pointer regionGrow = ConnectedFilterType::New();
			regionGrow->SetInput(smoother->GetOutput());
			regionGrow->SetReplaceValue(255);
			regionGrow->SetLower(lowerThreshold);
			regionGrow->SetUpper(upperThreshold);

			// Seed point
			ImageType::IndexType seedIndex;
			seedIndex[0] = seedX;
			seedIndex[1] = seedY;
			regionGrow->SetSeed(seedIndex);

			// Writer
			WriterType::Pointer writer = WriterType::New();
			writer->SetFileName(fileName + std::to_string(i - 6) + extension);
			writer->SetImageIO(dicomIO);
			writer->SetInput(regionGrow->GetOutput());
			writer->Update();

			std::cout << "Finished CT liver segmentation with a unique seed" << std::endl;
		}
		catch (itk::ExceptionObject & err) {
			std::cout << "\n\tCaught an exception!" << std::endl;
			std::cout << "\t" << err << std::endl;
			continue;
			//return EXIT_FAILURE;
		}
	}

}