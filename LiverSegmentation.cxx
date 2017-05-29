#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkConfidenceConnectedImageFilter.h"

typedef double FloatPixelType;
const unsigned int Dimension = 2;
typedef itk::Image< FloatPixelType, Dimension > InputImage;
typedef itk::Image< FloatPixelType, Dimension > OutputImage;

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << std::endl;
		std::cerr << argv[0];
		std::cerr << " input output inverse";
		std::cerr << std::endl;
		return EXIT_FAILURE;
	}

	const char * inputFileName = argv[1];
	const char * outputFileName = argv[2];

	try {
		typedef itk::ImageFileWriter< OutputImage >  WriterType;
		typedef itk::ImageFileReader< InputImage >  ReaderType;
		ReaderType::Pointer reader = ReaderType::New();
		reader->SetFileName(inputFileName);
		reader->Update();

		typedef itk::ConfidenceConnectedImageFilter<InputImage, InputImage> ConfidenceConnectedFilterType;
		ConfidenceConnectedFilterType::Pointer confidenceConnectedFilter = ConfidenceConnectedFilterType::New();
		confidenceConnectedFilter->SetInitialNeighborhoodRadius(3);
		confidenceConnectedFilter->SetMultiplier(3);
		confidenceConnectedFilter->SetNumberOfIterations(5);
		confidenceConnectedFilter->SetReplaceValue(255);

		// Set seed
		InputImage::IndexType seed;
		seed[0] = atoi("50");
		seed[1] = atoi("100");
		confidenceConnectedFilter->SetSeed(seed);
		confidenceConnectedFilter->SetInput(reader->GetOutput());


		WriterType::Pointer writer = WriterType::New();
		writer->SetFileName(inputFileName);
		writer->SetInput(confidenceConnectedFilter->GetOutput());
		writer->Update();
	}
	catch (itk::ExceptionObject & err) {
		std::cout << "Caught an exception!" << std::endl;
		std::cout << err << std::endl;
		return EXIT_FAILURE;
	}
}