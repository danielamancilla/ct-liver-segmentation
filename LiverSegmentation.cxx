#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkConfidenceConnectedImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

typedef signed short PixelType;
const unsigned int Dimension = 2;
typedef itk::Image< PixelType, Dimension > ImageType;

/*
**
** CT Liver segmentation
**
** D. Mancilla, D. Pedraza
** Pontificia Universidad Javeriana, 2017
**
**/

int main( int argc, char* argv[] ) {
	if ( argc != 3 ) {
		std::cerr << "Usage: " << std::endl;
		std::cerr << argv[0];
		std::cerr << " input output";
		std::cerr << std::endl;
		return EXIT_FAILURE;
	}

	const char * inputFileName = argv[1];
	const char * outputFileName = argv[2];

	try {

		// Reader for DICOM slice
		typedef itk::ImageFileWriter< ImageType >  WriterType;
		typedef itk::ImageFileReader< ImageType >  ReaderType;
		ReaderType::Pointer reader = ReaderType::New();
		reader->SetFileName( inputFileName );

		typedef itk::GDCMImageIO ImageIOType;
		ImageIOType::Pointer dicomIO = ImageIOType::New();
		reader->SetImageIO( dicomIO );
		reader->Update();

		typedef itk::ConfidenceConnectedImageFilter< ImageType, ImageType > ConfidenceConnectedFilterType;
		ConfidenceConnectedFilterType::Pointer confidenceConnectedFilter = ConfidenceConnectedFilterType::New();
		confidenceConnectedFilter->SetInitialNeighborhoodRadius(3);
		confidenceConnectedFilter->SetMultiplier(3);
		confidenceConnectedFilter->SetNumberOfIterations(5);
		confidenceConnectedFilter->SetReplaceValue(255);

		// Set seed
		ImageType::IndexType seed;
		seed[0] = atoi( "50" );
		seed[1] = atoi( "100" );
		confidenceConnectedFilter->SetSeed( seed );
		confidenceConnectedFilter->SetInput( reader->GetOutput() );

		WriterType::Pointer writer = WriterType::New();
		writer->SetFileName( outputFileName );
		writer->SetImageIO( dicomIO );
		writer->SetInput( confidenceConnectedFilter->GetOutput( ) );
		writer->Update();
	}
	catch (itk::ExceptionObject & err) {
		std::cout << "\n\tCaught an exception!" << std::endl;
		std::cout << "\t" << err << std::endl;
		return EXIT_FAILURE;
	}
}