/**
 *
 *  @file			exportXML.h
 *  @brief          Export SQL structure to XML MPEG-7 file
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__exportXML__
#define __Orchids__exportXML__

#include "Orchids.h"

/**
 *  @brief Write the default MPEG-7 XML header
 */
void writeXMLHeader(FILE *f);
/**
 *  @brief Export a matrixPtr to XML
 */
void writeMatrixtoXML(matrixPtr matrix, FILE *f);
/**
 *  @brief Export IRCAM descriptors to XML
 */
void writeDescriptortoXML(feature<float> descriptor, FILE *f);
/**
 *  @brief Export partials to XML
 */
void writePartialstoXML(partials partial, FILE *f);
/**
 *  @brief Export mel bands to XML
 */
void writeMelBandstoXML(melStruct mel, FILE *f);

/**
 *  @brief Create an XML file with the given filepath and soundfile and launch export
 */
int exportXML(sqlStruct sqlFeatures, string filepath, string soundfile);
/**
 *  @brief Create an XML file with the given soundfile in the current directory and launch export
 */
int exportXML(sqlStruct sqlFeatures, string soundfile);

#endif
