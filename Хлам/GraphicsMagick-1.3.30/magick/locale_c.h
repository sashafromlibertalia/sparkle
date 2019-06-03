#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 575

#define MGK_BlobErrorUnableToCreateBlob 1
#define MGK_BlobErrorUnableToDeduceImageFormat 2
#define MGK_BlobErrorUnableToObtainOffset 3
#define MGK_BlobErrorUnableToOpenFile 4
#define MGK_BlobErrorUnableToReadFile 5
#define MGK_BlobErrorUnableToReadToOffset 6
#define MGK_BlobErrorUnableToSeekToOffset 7
#define MGK_BlobErrorUnableToWriteBlob 8
#define MGK_BlobErrorUnrecognizedImageFormat 9
#define MGK_BlobFatalErrorDefault 10
#define MGK_BlobWarningDefault 11
#define MGK_CacheErrorInconsistentPersistentCacheDepth 12
#define MGK_CacheErrorPixelCacheDimensionsMisMatch 13
#define MGK_CacheErrorPixelCacheIsNotOpen 14
#define MGK_CacheErrorUnableToAllocateCacheView 15
#define MGK_CacheErrorUnableToCloneCache 16
#define MGK_CacheErrorUnableToExtendCache 17
#define MGK_CacheErrorUnableToGetCacheNexus 18
#define MGK_CacheErrorUnableToGetPixelsFromCache 19
#define MGK_CacheErrorUnableToOpenCache 20
#define MGK_CacheErrorUnableToPeristPixelCache 21
#define MGK_CacheErrorUnableToReadPixelCache 22
#define MGK_CacheErrorUnableToSyncCache 23
#define MGK_CacheFatalErrorDiskAllocationFailed 24
#define MGK_CacheFatalErrorUnableToExtendPixelCache 25
#define MGK_CacheWarningDefault 26
#define MGK_CoderErrorArithmeticOverflow 27
#define MGK_CoderErrorColormapTooLarge 28
#define MGK_CoderErrorColormapTypeNotSupported 29
#define MGK_CoderErrorColorspaceModelIsNotSupported 30
#define MGK_CoderErrorColorTypeNotSupported 31
#define MGK_CoderErrorCompressionNotValid 32
#define MGK_CoderErrorDataEncodingSchemeIsNotSupported 33
#define MGK_CoderErrorDataStorageTypeIsNotSupported 34
#define MGK_CoderErrorDecodedImageNotReturned 35
#define MGK_CoderErrorDeltaPNGNotSupported 36
#define MGK_CoderErrorDivisionByZero 37
#define MGK_CoderErrorEncryptedWPGImageFileNotSupported 38
#define MGK_CoderErrorFractalCompressionNotSupported 39
#define MGK_CoderErrorImageColumnOrRowSizeIsNotSupported 40
#define MGK_CoderErrorImageDoesNotHaveAMatteChannel 41
#define MGK_CoderErrorImageIsNotTiled 42
#define MGK_CoderErrorImageTypeNotSupported 43
#define MGK_CoderErrorIncompatibleSizeOfDouble 44
#define MGK_CoderErrorIrregularChannelGeometryNotSupported 45
#define MGK_CoderErrorJNGCompressionNotSupported 46
#define MGK_CoderErrorJPEGCompressionNotSupported 47
#define MGK_CoderErrorJPEGEmbeddingFailed 48
#define MGK_CoderErrorLocationTypeIsNotSupported 49
#define MGK_CoderErrorMapStorageTypeIsNotSupported 50
#define MGK_CoderErrorMSBByteOrderNotSupported 51
#define MGK_CoderErrorMultidimensionalMatricesAreNotSupported 52
#define MGK_CoderErrorMultipleRecordListNotSupported 53
#define MGK_CoderErrorNo8BIMDataIsAvailable 54
#define MGK_CoderErrorNoAPP1DataIsAvailable 55
#define MGK_CoderErrorNoBitmapOnClipboard 56
#define MGK_CoderErrorNoColorProfileAvailable 57
#define MGK_CoderErrorNoDataReturned 58
#define MGK_CoderErrorNoImageVectorGraphics 59
#define MGK_CoderErrorNoIPTCInfoWasFound 60
#define MGK_CoderErrorNoIPTCProfileAvailable 61
#define MGK_CoderErrorNumberOfImagesIsNotSupported 62
#define MGK_CoderErrorOnlyContinuousTonePictureSupported 63
#define MGK_CoderErrorOnlyLevelZerofilesSupported 64
#define MGK_CoderErrorPNGCompressionNotSupported 65
#define MGK_CoderErrorPNGLibraryTooOld 66
#define MGK_CoderErrorRLECompressionNotSupported 67
#define MGK_CoderErrorSubsamplingRequiresEvenWidth 68
#define MGK_CoderErrorUnableToCopyProfile 69
#define MGK_CoderErrorUnableToCreateADC 70
#define MGK_CoderErrorUnableToCreateBitmap 71
#define MGK_CoderErrorUnableToDecompressImage 72
#define MGK_CoderErrorUnableToInitializeFPXLibrary 73
#define MGK_CoderErrorUnableToOpenBlob 74
#define MGK_CoderErrorUnableToReadAspectRatio 75
#define MGK_CoderErrorUnableToReadCIELABImages 76
#define MGK_CoderErrorUnableToReadSummaryInfo 77
#define MGK_CoderErrorUnableToSetAffineMatrix 78
#define MGK_CoderErrorUnableToSetAspectRatio 79
#define MGK_CoderErrorUnableToSetColorTwist 80
#define MGK_CoderErrorUnableToSetContrast 81
#define MGK_CoderErrorUnableToSetFilteringValue 82
#define MGK_CoderErrorUnableToSetImageComments 83
#define MGK_CoderErrorUnableToSetImageTitle 84
#define MGK_CoderErrorUnableToSetJPEGLevel 85
#define MGK_CoderErrorUnableToSetRegionOfInterest 86
#define MGK_CoderErrorUnableToSetSummaryInfo 87
#define MGK_CoderErrorUnableToTranslateText 88
#define MGK_CoderErrorUnableToWriteMPEGParameters 89
#define MGK_CoderErrorUnableToWriteTemporaryFile 90
#define MGK_CoderErrorUnableToZipCompressImage 91
#define MGK_CoderErrorUnsupportedBitsPerSample 92
#define MGK_CoderErrorUnsupportedCellTypeInTheMatrix 93
#define MGK_CoderErrorUnsupportedSamplesPerPixel 94
#define MGK_CoderErrorWebPDecodingFailedUserAbort 95
#define MGK_CoderErrorWebPEncodingFailed 96
#define MGK_CoderErrorWebPEncodingFailedBadDimension 97
#define MGK_CoderErrorWebPEncodingFailedBadWrite 98
#define MGK_CoderErrorWebPEncodingFailedBitstreamOutOfMemory 99
#define MGK_CoderErrorWebPEncodingFailedFileTooBig 100
#define MGK_CoderErrorWebPEncodingFailedInvalidConfiguration 101
#define MGK_CoderErrorWebPEncodingFailedNULLParameter 102
#define MGK_CoderErrorWebPEncodingFailedOutOfMemory 103
#define MGK_CoderErrorWebPEncodingFailedPartition0Overflow 104
#define MGK_CoderErrorWebPEncodingFailedPartitionOverflow 105
#define MGK_CoderErrorWebPEncodingFailedUserAbort 106
#define MGK_CoderErrorWebPInvalidConfiguration 107
#define MGK_CoderErrorWebPInvalidParameter 108
#define MGK_CoderErrorZipCompressionNotSupported 109
#define MGK_CoderFatalErrorDefault 110
#define MGK_CoderWarningLosslessToLossyJPEGConversion 111
#define MGK_ConfigureErrorIncludeElementNestedTooDeeply 112
#define MGK_ConfigureErrorRegistryKeyLookupFailed 113
#define MGK_ConfigureErrorStringTokenLengthExceeded 114
#define MGK_ConfigureErrorUnableToAccessConfigureFile 115
#define MGK_ConfigureErrorUnableToAccessFontFile 116
#define MGK_ConfigureErrorUnableToAccessLogFile 117
#define MGK_ConfigureErrorUnableToAccessModuleFile 118
#define MGK_ConfigureFatalErrorDefault 119
#define MGK_ConfigureFatalErrorUnableToChangeToWorkingDirectory 120
#define MGK_ConfigureFatalErrorUnableToGetCurrentDirectory 121
#define MGK_ConfigureFatalErrorUnableToRestoreCurrentDirectory 122
#define MGK_ConfigureWarningDefault 123
#define MGK_CorruptImageErrorAnErrorHasOccurredReadingFromFile 124
#define MGK_CorruptImageErrorAnErrorHasOccurredWritingToFile 125
#define MGK_CorruptImageErrorColormapExceedsColorsLimit 126
#define MGK_CorruptImageErrorCompressionNotValid 127
#define MGK_CorruptImageErrorCorruptImage 128
#define MGK_CorruptImageErrorImageFileDoesNotContainAnyImageData 129
#define MGK_CorruptImageErrorImageFileHasNoScenes 130
#define MGK_CorruptImageErrorImageTypeNotSupported 131
#define MGK_CorruptImageErrorImproperImageHeader 132
#define MGK_CorruptImageErrorInsufficientImageDataInFile 133
#define MGK_CorruptImageErrorInvalidColormapIndex 134
#define MGK_CorruptImageErrorInvalidFileFormatVersion 135
#define MGK_CorruptImageErrorLengthAndFilesizeDoNotMatch 136
#define MGK_CorruptImageErrorMissingImageChannel 137
#define MGK_CorruptImageErrorNegativeOrZeroImageSize 138
#define MGK_CorruptImageErrorNonOS2HeaderSizeError 139
#define MGK_CorruptImageErrorNotEnoughTiles 140
#define MGK_CorruptImageErrorStaticPlanesValueNotEqualToOne 141
#define MGK_CorruptImageErrorSubsamplingRequiresEvenWidth 142
#define MGK_CorruptImageErrorTooMuchImageDataInFile 143
#define MGK_CorruptImageErrorUnableToReadColormapFromDumpFile 144
#define MGK_CorruptImageErrorUnableToReadColorProfile 145
#define MGK_CorruptImageErrorUnableToReadExtensionBlock 146
#define MGK_CorruptImageErrorUnableToReadGenericProfile 147
#define MGK_CorruptImageErrorUnableToReadImageData 148
#define MGK_CorruptImageErrorUnableToReadImageHeader 149
#define MGK_CorruptImageErrorUnableToReadIPTCProfile 150
#define MGK_CorruptImageErrorUnableToReadPixmapFromDumpFile 151
#define MGK_CorruptImageErrorUnableToReadSubImageData 152
#define MGK_CorruptImageErrorUnableToReadVIDImage 153
#define MGK_CorruptImageErrorUnableToReadWindowNameFromDumpFile 154
#define MGK_CorruptImageErrorUnableToRunlengthDecodeImage 155
#define MGK_CorruptImageErrorUnableToUncompressImage 156
#define MGK_CorruptImageErrorUnexpectedEndOfFile 157
#define MGK_CorruptImageErrorUnexpectedSamplingFactor 158
#define MGK_CorruptImageErrorUnknownPatternType 159
#define MGK_CorruptImageErrorUnrecognizedBitsPerPixel 160
#define MGK_CorruptImageErrorUnrecognizedImageCompression 161
#define MGK_CorruptImageErrorUnrecognizedNumberOfColors 162
#define MGK_CorruptImageErrorUnrecognizedXWDHeader 163
#define MGK_CorruptImageErrorUnsupportedBitsPerSample 164
#define MGK_CorruptImageErrorUnsupportedNumberOfPlanes 165
#define MGK_CorruptImageFatalErrorUnableToPersistKey 166
#define MGK_CorruptImageWarningCompressionNotValid 167
#define MGK_CorruptImageWarningCorruptImage 168
#define MGK_CorruptImageWarningImproperImageHeader 169
#define MGK_CorruptImageWarningInvalidColormapIndex 170
#define MGK_CorruptImageWarningLengthAndFilesizeDoNotMatch 171
#define MGK_CorruptImageWarningNegativeOrZeroImageSize 172
#define MGK_CorruptImageWarningNonOS2HeaderSizeError 173
#define MGK_CorruptImageWarningSkipToSyncByte 174
#define MGK_CorruptImageWarningStaticPlanesValueNotEqualToOne 175
#define MGK_CorruptImageWarningUnableToParseEmbeddedProfile 176
#define MGK_CorruptImageWarningUnrecognizedBitsPerPixel 177
#define MGK_CorruptImageWarningUnrecognizedImageCompression 178
#define MGK_DelegateErrorDelegateFailed 179
#define MGK_DelegateErrorFailedToAllocateArgumentList 180
#define MGK_DelegateErrorFailedToAllocateGhostscriptInterpreter 181
#define MGK_DelegateErrorFailedToComputeOutputSize 182
#define MGK_DelegateErrorFailedToFindGhostscript 183
#define MGK_DelegateErrorFailedToRenderFile 184
#define MGK_DelegateErrorFailedToScanFile 185
#define MGK_DelegateErrorNoTagFound 186
#define MGK_DelegateErrorPostscriptDelegateFailed 187
#define MGK_DelegateErrorUnableToCreateImage 188
#define MGK_DelegateErrorUnableToCreateImageComponent 189
#define MGK_DelegateErrorUnableToDecodeImageFile 190
#define MGK_DelegateErrorUnableToEncodeImageFile 191
#define MGK_DelegateErrorUnableToInitializeFPXLibrary 192
#define MGK_DelegateErrorUnableToInitializeWMFLibrary 193
#define MGK_DelegateErrorUnableToManageJP2Stream 194
#define MGK_DelegateErrorUnableToWriteSVGFormat 195
#define MGK_DelegateErrorWebPABIMismatch 196
#define MGK_DelegateFatalErrorDefault 197
#define MGK_DelegateWarningDefault 198
#define MGK_DrawErrorAlreadyPushingPatternDefinition 199
#define MGK_DrawErrorDrawingRecursionDetected 200
#define MGK_DrawErrorFloatValueConversionError 201
#define MGK_DrawErrorIntegerValueConversionError 202
#define MGK_DrawErrorInvalidPrimitiveArgument 203
#define MGK_DrawErrorNonconformingDrawingPrimitiveDefinition 204
#define MGK_DrawErrorPrimitiveArithmeticOverflow 205
#define MGK_DrawErrorTooManyCoordinates 206
#define MGK_DrawErrorUnableToPrint 207
#define MGK_DrawErrorUnbalancedGraphicContextPushPop 208
#define MGK_DrawErrorUnreasonableGradientSize 209
#define MGK_DrawErrorVectorPathTruncated 210
#define MGK_DrawFatalErrorDefault 211
#define MGK_DrawWarningNotARelativeURL 212
#define MGK_DrawWarningNotCurrentlyPushingPatternDefinition 213
#define MGK_DrawWarningURLNotFound 214
#define MGK_FileOpenErrorUnableToCreateTemporaryFile 215
#define MGK_FileOpenErrorUnableToOpenFile 216
#define MGK_FileOpenErrorUnableToWriteFile 217
#define MGK_FileOpenFatalErrorDefault 218
#define MGK_FileOpenWarningDefault 219
#define MGK_ImageErrorAngleIsDiscontinuous 220
#define MGK_ImageErrorCMYKAImageLacksAlphaChannel 221
#define MGK_ImageErrorColorspaceColorProfileMismatch 222
#define MGK_ImageErrorImageColorspaceDiffers 223
#define MGK_ImageErrorImageColorspaceMismatch 224
#define MGK_ImageErrorImageDifferenceExceedsLimit 225
#define MGK_ImageErrorImageDoesNotContainResolution 226
#define MGK_ImageErrorImageIsNotColormapped 227
#define MGK_ImageErrorImageOpacityDiffers 228
#define MGK_ImageErrorImageSequenceIsRequired 229
#define MGK_ImageErrorImageSizeDiffers 230
#define MGK_ImageErrorInvalidColormapIndex 231
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 232
#define MGK_ImageErrorNoImagesWereFound 233
#define MGK_ImageErrorNoImagesWereLoaded 234
#define MGK_ImageErrorNoLocaleImageAttribute 235
#define MGK_ImageErrorTooManyClusters 236
#define MGK_ImageErrorUnableToAppendImage 237
#define MGK_ImageErrorUnableToAssignProfile 238
#define MGK_ImageErrorUnableToAverageImage 239
#define MGK_ImageErrorUnableToCoalesceImage 240
#define MGK_ImageErrorUnableToCompareImages 241
#define MGK_ImageErrorUnableToCreateImageMosaic 242
#define MGK_ImageErrorUnableToCreateStereoImage 243
#define MGK_ImageErrorUnableToDeconstructImageSequence 244
#define MGK_ImageErrorUnableToExportImagePixels 245
#define MGK_ImageErrorUnableToFlattenImage 246
#define MGK_ImageErrorUnableToGetClipMask 247
#define MGK_ImageErrorUnableToGetCompositeMask 248
#define MGK_ImageErrorUnableToHandleImageChannel 249
#define MGK_ImageErrorUnableToImportImagePixels 250
#define MGK_ImageErrorUnableToResizeImage 251
#define MGK_ImageErrorUnableToSegmentImage 252
#define MGK_ImageErrorUnableToSetClipMask 253
#define MGK_ImageErrorUnableToSetCompositeMask 254
#define MGK_ImageErrorUnableToShearImage 255
#define MGK_ImageErrorWidthOrHeightExceedsLimit 256
#define MGK_ImageFatalErrorUnableToPersistKey 257
#define MGK_ImageWarningDefault 258
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 259
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 260
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 261
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 262
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 263
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 264
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 265
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 266
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 267
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 268
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 269
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 270
#define MGK_MissingDelegateFatalErrorDefault 271
#define MGK_MissingDelegateWarningDefault 272
#define MGK_ModuleErrorFailedToCloseModule 273
#define MGK_ModuleErrorFailedToFindSymbol 274
#define MGK_ModuleErrorUnableToLoadModule 275
#define MGK_ModuleErrorUnableToRegisterImageFormat 276
#define MGK_ModuleErrorUnrecognizedModule 277
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 278
#define MGK_ModuleWarningDefault 279
#define MGK_MonitorErrorDefault 280
#define MGK_MonitorFatalErrorDefault 281
#define MGK_MonitorFatalErrorUserRequestedTerminationBySignal 282
#define MGK_MonitorWarningDefault 283
#define MGK_OptionErrorBevelWidthIsNegative 284
#define MGK_OptionErrorColorSeparatedImageRequired 285
#define MGK_OptionErrorFrameIsLessThanImageSize 286
#define MGK_OptionErrorGeometryDimensionsAreZero 287
#define MGK_OptionErrorGeometryDoesNotContainImage 288
#define MGK_OptionErrorHaldClutImageDimensionsInvalid 289
#define MGK_OptionErrorImagesAreNotTheSameSize 290
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 291
#define MGK_OptionErrorImageSmallerThanKernelWidth 292
#define MGK_OptionErrorImageSmallerThanRadius 293
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 294
#define MGK_OptionErrorInputImagesAlreadySpecified 295
#define MGK_OptionErrorInvalidSubimageSpecification 296
#define MGK_OptionErrorKernelRadiusIsTooSmall 297
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 298
#define MGK_OptionErrorMatrixIsNotSquare 299
#define MGK_OptionErrorMatrixOrderOutOfRange 300
#define MGK_OptionErrorMissingAnImageFilename 301
#define MGK_OptionErrorMissingArgument 302
#define MGK_OptionErrorMustSpecifyAnImageName 303
#define MGK_OptionErrorMustSpecifyImageSize 304
#define MGK_OptionErrorNoBlobDefined 305
#define MGK_OptionErrorNoImagesDefined 306
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 307
#define MGK_OptionErrorNoProfileNameWasGiven 308
#define MGK_OptionErrorNullBlobArgument 309
#define MGK_OptionErrorReferenceImageRequired 310
#define MGK_OptionErrorReferenceIsNotMyType 311
#define MGK_OptionErrorRegionAreaExceedsLimit 312
#define MGK_OptionErrorRequestDidNotReturnAnImage 313
#define MGK_OptionErrorSteganoImageRequired 314
#define MGK_OptionErrorStereoImageRequired 315
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 316
#define MGK_OptionErrorTileNotBoundedByImageDimensions 317
#define MGK_OptionErrorUnableToAddOrRemoveProfile 318
#define MGK_OptionErrorUnableToAverageImageSequence 319
#define MGK_OptionErrorUnableToBlurImage 320
#define MGK_OptionErrorUnableToChopImage 321
#define MGK_OptionErrorUnableToColorMatrixImage 322
#define MGK_OptionErrorUnableToConstituteImage 323
#define MGK_OptionErrorUnableToConvolveImage 324
#define MGK_OptionErrorUnableToEdgeImage 325
#define MGK_OptionErrorUnableToEqualizeImage 326
#define MGK_OptionErrorUnableToFilterImage 327
#define MGK_OptionErrorUnableToFormatImageMetadata 328
#define MGK_OptionErrorUnableToFrameImage 329
#define MGK_OptionErrorUnableToOilPaintImage 330
#define MGK_OptionErrorUnableToPaintImage 331
#define MGK_OptionErrorUnableToRaiseImage 332
#define MGK_OptionErrorUnableToSharpenImage 333
#define MGK_OptionErrorUnableToThresholdImage 334
#define MGK_OptionErrorUnableToWaveImage 335
#define MGK_OptionErrorUnrecognizedAttribute 336
#define MGK_OptionErrorUnrecognizedChannelType 337
#define MGK_OptionErrorUnrecognizedColor 338
#define MGK_OptionErrorUnrecognizedColormapType 339
#define MGK_OptionErrorUnrecognizedColorspace 340
#define MGK_OptionErrorUnrecognizedCommand 341
#define MGK_OptionErrorUnrecognizedComposeOperator 342
#define MGK_OptionErrorUnrecognizedDisposeMethod 343
#define MGK_OptionErrorUnrecognizedElement 344
#define MGK_OptionErrorUnrecognizedEndianType 345
#define MGK_OptionErrorUnrecognizedGravityType 346
#define MGK_OptionErrorUnrecognizedHighlightStyle 347
#define MGK_OptionErrorUnrecognizedImageCompression 348
#define MGK_OptionErrorUnrecognizedImageFilter 349
#define MGK_OptionErrorUnrecognizedImageFormat 350
#define MGK_OptionErrorUnrecognizedImageMode 351
#define MGK_OptionErrorUnrecognizedImageType 352
#define MGK_OptionErrorUnrecognizedIntentType 353
#define MGK_OptionErrorUnrecognizedInterlaceType 354
#define MGK_OptionErrorUnrecognizedListType 355
#define MGK_OptionErrorUnrecognizedMetric 356
#define MGK_OptionErrorUnrecognizedModeType 357
#define MGK_OptionErrorUnrecognizedNoiseType 358
#define MGK_OptionErrorUnrecognizedOperator 359
#define MGK_OptionErrorUnrecognizedOption 360
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 361
#define MGK_OptionErrorUnrecognizedPixelMap 362
#define MGK_OptionErrorUnrecognizedPreviewType 363
#define MGK_OptionErrorUnrecognizedResourceType 364
#define MGK_OptionErrorUnrecognizedType 365
#define MGK_OptionErrorUnrecognizedUnitsType 366
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 367
#define MGK_OptionErrorUnsupportedSamplingFactor 368
#define MGK_OptionErrorUsageError 369
#define MGK_OptionFatalErrorInvalidColorspaceType 370
#define MGK_OptionFatalErrorInvalidEndianType 371
#define MGK_OptionFatalErrorInvalidImageType 372
#define MGK_OptionFatalErrorInvalidInterlaceType 373
#define MGK_OptionFatalErrorMissingAnImageFilename 374
#define MGK_OptionFatalErrorMissingArgument 375
#define MGK_OptionFatalErrorNoImagesWereLoaded 376
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 377
#define MGK_OptionFatalErrorRequestDidNotReturnAnImage 378
#define MGK_OptionFatalErrorUnableToOpenXServer 379
#define MGK_OptionFatalErrorUnableToPersistKey 380
#define MGK_OptionFatalErrorUnrecognizedColormapType 381
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 382
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 383
#define MGK_OptionFatalErrorUnrecognizedEndianType 384
#define MGK_OptionFatalErrorUnrecognizedFilterType 385
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 386
#define MGK_OptionFatalErrorUnrecognizedImageType 387
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 388
#define MGK_OptionFatalErrorUnrecognizedOption 389
#define MGK_OptionFatalErrorUnrecognizedResourceType 390
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 391
#define MGK_OptionWarningUnrecognizedColor 392
#define MGK_RegistryErrorImageExpected 393
#define MGK_RegistryErrorImageInfoExpected 394
#define MGK_RegistryErrorStructureSizeMismatch 395
#define MGK_RegistryErrorUnableToGetRegistryID 396
#define MGK_RegistryErrorUnableToLocateImage 397
#define MGK_RegistryErrorUnableToSetRegistry 398
#define MGK_RegistryFatalErrorDefault 399
#define MGK_RegistryWarningDefault 400
#define MGK_ResourceLimitErrorCacheResourcesExhausted 401
#define MGK_ResourceLimitErrorImagePixelHeightLimitExceeded 402
#define MGK_ResourceLimitErrorImagePixelLimitExceeded 403
#define MGK_ResourceLimitErrorImagePixelWidthLimitExceeded 404
#define MGK_ResourceLimitErrorMemoryAllocationFailed 405
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 406
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 407
#define MGK_ResourceLimitErrorUnableToAddColorProfile 408
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 409
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 410
#define MGK_ResourceLimitErrorUnableToAddOrRemoveProfile 411
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 412
#define MGK_ResourceLimitErrorUnableToAllocateColormap 413
#define MGK_ResourceLimitErrorUnableToAllocateICCProfile 414
#define MGK_ResourceLimitErrorUnableToAllocateImage 415
#define MGK_ResourceLimitErrorUnableToAllocateString 416
#define MGK_ResourceLimitErrorUnableToAnnotateImage 417
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 418
#define MGK_ResourceLimitErrorUnableToCloneDrawingWand 419
#define MGK_ResourceLimitErrorUnableToCloneImage 420
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 421
#define MGK_ResourceLimitErrorUnableToConstituteImage 422
#define MGK_ResourceLimitErrorUnableToConvertFont 423
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 424
#define MGK_ResourceLimitErrorUnableToCreateColormap 425
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 426
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 427
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 428
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 429
#define MGK_ResourceLimitErrorUnableToCreateXWindow 430
#define MGK_ResourceLimitErrorUnableToCropImage 431
#define MGK_ResourceLimitErrorUnableToDespeckleImage 432
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 433
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 434
#define MGK_ResourceLimitErrorUnableToDitherImage 435
#define MGK_ResourceLimitErrorUnableToDrawOnImage 436
#define MGK_ResourceLimitErrorUnableToEdgeImage 437
#define MGK_ResourceLimitErrorUnableToEmbossImage 438
#define MGK_ResourceLimitErrorUnableToEnhanceImage 439
#define MGK_ResourceLimitErrorUnableToFloodfillImage 440
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 441
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 442
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 443
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 444
#define MGK_ResourceLimitErrorUnableToLevelImage 445
#define MGK_ResourceLimitErrorUnableToMagnifyImage 446
#define MGK_ResourceLimitErrorUnableToManageColor 447
#define MGK_ResourceLimitErrorUnableToMapImage 448
#define MGK_ResourceLimitErrorUnableToMapImageSequence 449
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 450
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 451
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 452
#define MGK_ResourceLimitErrorUnableToNormalizeImage 453
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 454
#define MGK_ResourceLimitErrorUnableToQuantizeImage 455
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 456
#define MGK_ResourceLimitErrorUnableToReadTextChunk 457
#define MGK_ResourceLimitErrorUnableToReadXImage 458
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 459
#define MGK_ResourceLimitErrorUnableToResizeImage 460
#define MGK_ResourceLimitErrorUnableToRotateImage 461
#define MGK_ResourceLimitErrorUnableToSampleImage 462
#define MGK_ResourceLimitErrorUnableToScaleImage 463
#define MGK_ResourceLimitErrorUnableToSelectImage 464
#define MGK_ResourceLimitErrorUnableToSharpenImage 465
#define MGK_ResourceLimitErrorUnableToShaveImage 466
#define MGK_ResourceLimitErrorUnableToShearImage 467
#define MGK_ResourceLimitErrorUnableToSortImageColormap 468
#define MGK_ResourceLimitErrorUnableToThresholdImage 469
#define MGK_ResourceLimitErrorUnableToTransformColorspace 470
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 471
#define MGK_ResourceLimitFatalErrorSemaporeOperationFailed 472
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 473
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 474
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 475
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 476
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 477
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 478
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 479
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 480
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 481
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 482
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 483
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 484
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 485
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 486
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 487
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 488
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickMap 489
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 490
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 491
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 492
#define MGK_ResourceLimitFatalErrorUnableToAllocateRandomKernel 493
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 494
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 495
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 496
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 497
#define MGK_ResourceLimitFatalErrorUnableToAllocateWand 498
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 499
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 500
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 501
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 502
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 503
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 504
#define MGK_ResourceLimitFatalErrorUnableToConvertText 505
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 506
#define MGK_ResourceLimitFatalErrorUnableToDestroySemaphore 507
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 508
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 509
#define MGK_ResourceLimitFatalErrorUnableToInitializeSemaphore 510
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 511
#define MGK_ResourceLimitFatalErrorUnableToLockSemaphore 512
#define MGK_ResourceLimitFatalErrorUnableToObtainRandomEntropy 513
#define MGK_ResourceLimitFatalErrorUnableToUnlockSemaphore 514
#define MGK_ResourceLimitWarningMemoryAllocationFailed 515
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 516
#define MGK_StreamErrorNoStreamHandlerIsDefined 517
#define MGK_StreamErrorPixelCacheIsNotOpen 518
#define MGK_StreamErrorUnableToAcquirePixelStream 519
#define MGK_StreamErrorUnableToSetPixelStream 520
#define MGK_StreamErrorUnableToSyncPixelStream 521
#define MGK_StreamFatalErrorDefault 522
#define MGK_StreamWarningDefault 523
#define MGK_TypeErrorFontNotSpecified 524
#define MGK_TypeErrorFontSubstitutionRequired 525
#define MGK_TypeErrorUnableToGetTypeMetrics 526
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 527
#define MGK_TypeErrorUnableToReadFont 528
#define MGK_TypeErrorUnrecognizedFontEncoding 529
#define MGK_TypeFatalErrorDefault 530
#define MGK_TypeWarningDefault 531
#define MGK_WandErrorInvalidColormapIndex 532
#define MGK_WandErrorWandAPINotImplemented 533
#define MGK_WandErrorWandContainsNoImageIndexs 534
#define MGK_WandErrorWandContainsNoImages 535
#define MGK_XServerErrorColorIsNotKnownToServer 536
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 537
#define MGK_XServerErrorStandardColormapIsNotInitialized 538
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 539
#define MGK_XServerErrorUnableToCreateBitmap 540
#define MGK_XServerErrorUnableToCreateColormap 541
#define MGK_XServerErrorUnableToCreatePixmap 542
#define MGK_XServerErrorUnableToCreateProperty 543
#define MGK_XServerErrorUnableToCreateStandardColormap 544
#define MGK_XServerErrorUnableToDisplayImageInfo 545
#define MGK_XServerErrorUnableToGetProperty 546
#define MGK_XServerErrorUnableToGetStandardColormap 547
#define MGK_XServerErrorUnableToGetVisual 548
#define MGK_XServerErrorUnableToGrabMouse 549
#define MGK_XServerErrorUnableToLoadFont 550
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 551
#define MGK_XServerErrorUnableToOpenXServer 552
#define MGK_XServerErrorUnableToReadXAttributes 553
#define MGK_XServerErrorUnableToReadXWindowImage 554
#define MGK_XServerErrorUnrecognizedColormapType 555
#define MGK_XServerErrorUnrecognizedGravityType 556
#define MGK_XServerErrorUnrecognizedVisualSpecifier 557
#define MGK_XServerFatalErrorUnableToAllocateXHints 558
#define MGK_XServerFatalErrorUnableToCreateCursor 559
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 560
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 561
#define MGK_XServerFatalErrorUnableToCreateTextProperty 562
#define MGK_XServerFatalErrorUnableToCreateXImage 563
#define MGK_XServerFatalErrorUnableToCreateXPixmap 564
#define MGK_XServerFatalErrorUnableToCreateXWindow 565
#define MGK_XServerFatalErrorUnableToDisplayImage 566
#define MGK_XServerFatalErrorUnableToDitherImage 567
#define MGK_XServerFatalErrorUnableToGetPixelInfo 568
#define MGK_XServerFatalErrorUnableToGetVisual 569
#define MGK_XServerFatalErrorUnableToLoadFont 570
#define MGK_XServerFatalErrorUnableToMakeXWindow 571
#define MGK_XServerFatalErrorUnableToOpenXServer 572
#define MGK_XServerFatalErrorUnableToViewFonts 573
#define MGK_XServerWarningUnableToGetVisual 574
#define MGK_XServerWarningUsingDefaultVisual 575

#endif

#if defined(_INCLUDE_CATEGORYMAP_TABLE_)
typedef struct _CategoryInfo{
  const char *name;
  int offset;
} CategoryInfo;

static const CategoryInfo category_map[] =
  {
    { "Blob", 0 },
    { "Cache", 3 },
    { "Coder", 6 },
    { "Configure", 9 },
    { "Corrupt/Image", 12 },
    { "Delegate", 15 },
    { "Draw", 18 },
    { "File/Open", 21 },
    { "Image", 24 },
    { "Missing/Delegate", 27 },
    { "Module", 30 },
    { "Monitor", 33 },
    { "Option", 36 },
    { "Registry", 39 },
    { "Resource/Limit", 42 },
    { "Stream", 45 },
    { "Type", 48 },
    { "Wand", 51 },
    { "XServer", 52 },
    { 0, 54 }
  };
#endif

#if defined(_INCLUDE_SEVERITYMAP_TABLE_)
typedef struct _SeverityInfo{
  const char *name;
  int offset;
  ExceptionType severityid;
} SeverityInfo;

static const SeverityInfo severity_map[] =
  {
    { "Blob/Error", 0, BlobError },
    { "Blob/FatalError", 9, BlobFatalError },
    { "Blob/Warning", 10, BlobWarning },
    { "Cache/Error", 11, CacheError },
    { "Cache/FatalError", 23, CacheFatalError },
    { "Cache/Warning", 25, CacheWarning },
    { "Coder/Error", 26, CoderError },
    { "Coder/FatalError", 109, CoderFatalError },
    { "Coder/Warning", 110, CoderWarning },
    { "Configure/Error", 111, ConfigureError },
    { "Configure/FatalError", 118, ConfigureFatalError },
    { "Configure/Warning", 122, ConfigureWarning },
    { "Corrupt/Image/Error", 123, CorruptImageError },
    { "Corrupt/Image/FatalError", 165, CorruptImageFatalError },
    { "Corrupt/Image/Warning", 166, CorruptImageWarning },
    { "Delegate/Error", 178, DelegateError },
    { "Delegate/FatalError", 196, DelegateFatalError },
    { "Delegate/Warning", 197, DelegateWarning },
    { "Draw/Error", 198, DrawError },
    { "Draw/FatalError", 210, DrawFatalError },
    { "Draw/Warning", 211, DrawWarning },
    { "File/Open/Error", 214, FileOpenError },
    { "File/Open/FatalError", 217, FileOpenFatalError },
    { "File/Open/Warning", 218, FileOpenWarning },
    { "Image/Error", 219, ImageError },
    { "Image/FatalError", 256, ImageFatalError },
    { "Image/Warning", 257, ImageWarning },
    { "Missing/Delegate/Error", 258, MissingDelegateError },
    { "Missing/Delegate/FatalError", 270, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 271, MissingDelegateWarning },
    { "Module/Error", 272, ModuleError },
    { "Module/FatalError", 277, ModuleFatalError },
    { "Module/Warning", 278, ModuleWarning },
    { "Monitor/Error", 279, MonitorError },
    { "Monitor/FatalError", 280, MonitorFatalError },
    { "Monitor/Warning", 282, MonitorWarning },
    { "Option/Error", 283, OptionError },
    { "Option/FatalError", 369, OptionFatalError },
    { "Option/Warning", 391, OptionWarning },
    { "Registry/Error", 392, RegistryError },
    { "Registry/FatalError", 398, RegistryFatalError },
    { "Registry/Warning", 399, RegistryWarning },
    { "Resource/Limit/Error", 400, ResourceLimitError },
    { "Resource/Limit/FatalError", 470, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 514, ResourceLimitWarning },
    { "Stream/Error", 515, StreamError },
    { "Stream/FatalError", 521, StreamFatalError },
    { "Stream/Warning", 522, StreamWarning },
    { "Type/Error", 523, TypeError },
    { "Type/FatalError", 529, TypeFatalError },
    { "Type/Warning", 530, TypeWarning },
    { "Wand/Error", 531, WandError },
    { "XServer/Error", 535, XServerError },
    { "XServer/FatalError", 557, XServerFatalError },
    { "XServer/Warning", 573, XServerWarning },
    { 0, 575, UndefinedException }
  };
#endif

#if defined(_INCLUDE_TAGMAP_TABLE_)
typedef struct _MessageInfo
{
  const char *name;
  int messageid;
} MessageInfo;

static const MessageInfo message_map[] =
  {
    { "UnableToCreateBlob", 1 },
    { "UnableToDeduceImageFormat", 2 },
    { "UnableToObtainOffset", 3 },
    { "UnableToOpenFile", 4 },
    { "UnableToReadFile", 5 },
    { "UnableToReadToOffset", 6 },
    { "UnableToSeekToOffset", 7 },
    { "UnableToWriteBlob", 8 },
    { "UnrecognizedImageFormat", 9 },
    { "Default", 10 },
    { "Default", 11 },
    { "InconsistentPersistentCacheDepth", 12 },
    { "PixelCacheDimensionsMisMatch", 13 },
    { "PixelCacheIsNotOpen", 14 },
    { "UnableToAllocateCacheView", 15 },
    { "UnableToCloneCache", 16 },
    { "UnableToExtendCache", 17 },
    { "UnableToGetCacheNexus", 18 },
    { "UnableToGetPixelsFromCache", 19 },
    { "UnableToOpenCache", 20 },
    { "UnableToPeristPixelCache", 21 },
    { "UnableToReadPixelCache", 22 },
    { "UnableToSyncCache", 23 },
    { "DiskAllocationFailed", 24 },
    { "UnableToExtendPixelCache", 25 },
    { "Default", 26 },
    { "ArithmeticOverflow", 27 },
    { "ColormapTooLarge", 28 },
    { "ColormapTypeNotSupported", 29 },
    { "ColorspaceModelIsNotSupported", 30 },
    { "ColorTypeNotSupported", 31 },
    { "CompressionNotValid", 32 },
    { "DataEncodingSchemeIsNotSupported", 33 },
    { "DataStorageTypeIsNotSupported", 34 },
    { "DecodedImageNotReturned", 35 },
    { "DeltaPNGNotSupported", 36 },
    { "DivisionByZero", 37 },
    { "EncryptedWPGImageFileNotSupported", 38 },
    { "FractalCompressionNotSupported", 39 },
    { "ImageColumnOrRowSizeIsNotSupported", 40 },
    { "ImageDoesNotHaveAMatteChannel", 41 },
    { "ImageIsNotTiled", 42 },
    { "ImageTypeNotSupported", 43 },
    { "IncompatibleSizeOfDouble", 44 },
    { "IrregularChannelGeometryNotSupported", 45 },
    { "JNGCompressionNotSupported", 46 },
    { "JPEGCompressionNotSupported", 47 },
    { "JPEGEmbeddingFailed", 48 },
    { "LocationTypeIsNotSupported", 49 },
    { "MapStorageTypeIsNotSupported", 50 },
    { "MSBByteOrderNotSupported", 51 },
    { "MultidimensionalMatricesAreNotSupported", 52 },
    { "MultipleRecordListNotSupported", 53 },
    { "No8BIMDataIsAvailable", 54 },
    { "NoAPP1DataIsAvailable", 55 },
    { "NoBitmapOnClipboard", 56 },
    { "NoColorProfileAvailable", 57 },
    { "NoDataReturned", 58 },
    { "NoImageVectorGraphics", 59 },
    { "NoIPTCInfoWasFound", 60 },
    { "NoIPTCProfileAvailable", 61 },
    { "NumberOfImagesIsNotSupported", 62 },
    { "OnlyContinuousTonePictureSupported", 63 },
    { "OnlyLevelZerofilesSupported", 64 },
    { "PNGCompressionNotSupported", 65 },
    { "PNGLibraryTooOld", 66 },
    { "RLECompressionNotSupported", 67 },
    { "SubsamplingRequiresEvenWidth", 68 },
    { "UnableToCopyProfile", 69 },
    { "UnableToCreateADC", 70 },
    { "UnableToCreateBitmap", 71 },
    { "UnableToDecompressImage", 72 },
    { "UnableToInitializeFPXLibrary", 73 },
    { "UnableToOpenBlob", 74 },
    { "UnableToReadAspectRatio", 75 },
    { "UnableToReadCIELABImages", 76 },
    { "UnableToReadSummaryInfo", 77 },
    { "UnableToSetAffineMatrix", 78 },
    { "UnableToSetAspectRatio", 79 },
    { "UnableToSetColorTwist", 80 },
    { "UnableToSetContrast", 81 },
    { "UnableToSetFilteringValue", 82 },
    { "UnableToSetImageComments", 83 },
    { "UnableToSetImageTitle", 84 },
    { "UnableToSetJPEGLevel", 85 },
    { "UnableToSetRegionOfInterest", 86 },
    { "UnableToSetSummaryInfo", 87 },
    { "UnableToTranslateText", 88 },
    { "UnableToWriteMPEGParameters", 89 },
    { "UnableToWriteTemporaryFile", 90 },
    { "UnableToZipCompressImage", 91 },
    { "UnsupportedBitsPerSample", 92 },
    { "UnsupportedCellTypeInTheMatrix", 93 },
    { "UnsupportedSamplesPerPixel", 94 },
    { "WebPDecodingFailedUserAbort", 95 },
    { "WebPEncodingFailed", 96 },
    { "WebPEncodingFailedBadDimension", 97 },
    { "WebPEncodingFailedBadWrite", 98 },
    { "WebPEncodingFailedBitstreamOutOfMemory", 99 },
    { "WebPEncodingFailedFileTooBig", 100 },
    { "WebPEncodingFailedInvalidConfiguration", 101 },
    { "WebPEncodingFailedNULLParameter", 102 },
    { "WebPEncodingFailedOutOfMemory", 103 },
    { "WebPEncodingFailedPartition0Overflow", 104 },
    { "WebPEncodingFailedPartitionOverflow", 105 },
    { "WebPEncodingFailedUserAbort", 106 },
    { "WebPInvalidConfiguration", 107 },
    { "WebPInvalidParameter", 108 },
    { "ZipCompressionNotSupported", 109 },
    { "Default", 110 },
    { "LosslessToLossyJPEGConversion", 111 },
    { "IncludeElementNestedTooDeeply", 112 },
    { "RegistryKeyLookupFailed", 113 },
    { "StringTokenLengthExceeded", 114 },
    { "UnableToAccessConfigureFile", 115 },
    { "UnableToAccessFontFile", 116 },
    { "UnableToAccessLogFile", 117 },
    { "UnableToAccessModuleFile", 118 },
    { "Default", 119 },
    { "UnableToChangeToWorkingDirectory", 120 },
    { "UnableToGetCurrentDirectory", 121 },
    { "UnableToRestoreCurrentDirectory", 122 },
    { "Default", 123 },
    { "AnErrorHasOccurredReadingFromFile", 124 },
    { "AnErrorHasOccurredWritingToFile", 125 },
    { "ColormapExceedsColorsLimit", 126 },
    { "CompressionNotValid", 127 },
    { "CorruptImage", 128 },
    { "ImageFileDoesNotContainAnyImageData", 129 },
    { "ImageFileHasNoScenes", 130 },
    { "ImageTypeNotSupported", 131 },
    { "ImproperImageHeader", 132 },
    { "InsufficientImageDataInFile", 133 },
    { "InvalidColormapIndex", 134 },
    { "InvalidFileFormatVersion", 135 },
    { "LengthAndFilesizeDoNotMatch", 136 },
    { "MissingImageChannel", 137 },
    { "NegativeOrZeroImageSize", 138 },
    { "NonOS2HeaderSizeError", 139 },
    { "NotEnoughTiles", 140 },
    { "StaticPlanesValueNotEqualToOne", 141 },
    { "SubsamplingRequiresEvenWidth", 142 },
    { "TooMuchImageDataInFile", 143 },
    { "UnableToReadColormapFromDumpFile", 144 },
    { "UnableToReadColorProfile", 145 },
    { "UnableToReadExtensionBlock", 146 },
    { "UnableToReadGenericProfile", 147 },
    { "UnableToReadImageData", 148 },
    { "UnableToReadImageHeader", 149 },
    { "UnableToReadIPTCProfile", 150 },
    { "UnableToReadPixmapFromDumpFile", 151 },
    { "UnableToReadSubImageData", 152 },
    { "UnableToReadVIDImage", 153 },
    { "UnableToReadWindowNameFromDumpFile", 154 },
    { "UnableToRunlengthDecodeImage", 155 },
    { "UnableToUncompressImage", 156 },
    { "UnexpectedEndOfFile", 157 },
    { "UnexpectedSamplingFactor", 158 },
    { "UnknownPatternType", 159 },
    { "UnrecognizedBitsPerPixel", 160 },
    { "UnrecognizedImageCompression", 161 },
    { "UnrecognizedNumberOfColors", 162 },
    { "UnrecognizedXWDHeader", 163 },
    { "UnsupportedBitsPerSample", 164 },
    { "UnsupportedNumberOfPlanes", 165 },
    { "UnableToPersistKey", 166 },
    { "CompressionNotValid", 167 },
    { "CorruptImage", 168 },
    { "ImproperImageHeader", 169 },
    { "InvalidColormapIndex", 170 },
    { "LengthAndFilesizeDoNotMatch", 171 },
    { "NegativeOrZeroImageSize", 172 },
    { "NonOS2HeaderSizeError", 173 },
    { "SkipToSyncByte", 174 },
    { "StaticPlanesValueNotEqualToOne", 175 },
    { "UnableToParseEmbeddedProfile", 176 },
    { "UnrecognizedBitsPerPixel", 177 },
    { "UnrecognizedImageCompression", 178 },
    { "DelegateFailed", 179 },
    { "FailedToAllocateArgumentList", 180 },
    { "FailedToAllocateGhostscriptInterpreter", 181 },
    { "FailedToComputeOutputSize", 182 },
    { "FailedToFindGhostscript", 183 },
    { "FailedToRenderFile", 184 },
    { "FailedToScanFile", 185 },
    { "NoTagFound", 186 },
    { "PostscriptDelegateFailed", 187 },
    { "UnableToCreateImage", 188 },
    { "UnableToCreateImageComponent", 189 },
    { "UnableToDecodeImageFile", 190 },
    { "UnableToEncodeImageFile", 191 },
    { "UnableToInitializeFPXLibrary", 192 },
    { "UnableToInitializeWMFLibrary", 193 },
    { "UnableToManageJP2Stream", 194 },
    { "UnableToWriteSVGFormat", 195 },
    { "WebPABIMismatch", 196 },
    { "Default", 197 },
    { "Default", 198 },
    { "AlreadyPushingPatternDefinition", 199 },
    { "DrawingRecursionDetected", 200 },
    { "FloatValueConversionError", 201 },
    { "IntegerValueConversionError", 202 },
    { "InvalidPrimitiveArgument", 203 },
    { "NonconformingDrawingPrimitiveDefinition", 204 },
    { "PrimitiveArithmeticOverflow", 205 },
    { "TooManyCoordinates", 206 },
    { "UnableToPrint", 207 },
    { "UnbalancedGraphicContextPushPop", 208 },
    { "UnreasonableGradientSize", 209 },
    { "VectorPathTruncated", 210 },
    { "Default", 211 },
    { "NotARelativeURL", 212 },
    { "NotCurrentlyPushingPatternDefinition", 213 },
    { "URLNotFound", 214 },
    { "UnableToCreateTemporaryFile", 215 },
    { "UnableToOpenFile", 216 },
    { "UnableToWriteFile", 217 },
    { "Default", 218 },
    { "Default", 219 },
    { "AngleIsDiscontinuous", 220 },
    { "CMYKAImageLacksAlphaChannel", 221 },
    { "ColorspaceColorProfileMismatch", 222 },
    { "ImageColorspaceDiffers", 223 },
    { "ImageColorspaceMismatch", 224 },
    { "ImageDifferenceExceedsLimit", 225 },
    { "ImageDoesNotContainResolution", 226 },
    { "ImageIsNotColormapped", 227 },
    { "ImageOpacityDiffers", 228 },
    { "ImageSequenceIsRequired", 229 },
    { "ImageSizeDiffers", 230 },
    { "InvalidColormapIndex", 231 },
    { "LeftAndRightImageSizesDiffer", 232 },
    { "NoImagesWereFound", 233 },
    { "NoImagesWereLoaded", 234 },
    { "NoLocaleImageAttribute", 235 },
    { "TooManyClusters", 236 },
    { "UnableToAppendImage", 237 },
    { "UnableToAssignProfile", 238 },
    { "UnableToAverageImage", 239 },
    { "UnableToCoalesceImage", 240 },
    { "UnableToCompareImages", 241 },
    { "UnableToCreateImageMosaic", 242 },
    { "UnableToCreateStereoImage", 243 },
    { "UnableToDeconstructImageSequence", 244 },
    { "UnableToExportImagePixels", 245 },
    { "UnableToFlattenImage", 246 },
    { "UnableToGetClipMask", 247 },
    { "UnableToGetCompositeMask", 248 },
    { "UnableToHandleImageChannel", 249 },
    { "UnableToImportImagePixels", 250 },
    { "UnableToResizeImage", 251 },
    { "UnableToSegmentImage", 252 },
    { "UnableToSetClipMask", 253 },
    { "UnableToSetCompositeMask", 254 },
    { "UnableToShearImage", 255 },
    { "WidthOrHeightExceedsLimit", 256 },
    { "UnableToPersistKey", 257 },
    { "Default", 258 },
    { "DPSLibraryIsNotAvailable", 259 },
    { "FPXLibraryIsNotAvailable", 260 },
    { "FreeTypeLibraryIsNotAvailable", 261 },
    { "JPEGLibraryIsNotAvailable", 262 },
    { "LCMSLibraryIsNotAvailable", 263 },
    { "LZWEncodingNotEnabled", 264 },
    { "NoDecodeDelegateForThisImageFormat", 265 },
    { "NoEncodeDelegateForThisImageFormat", 266 },
    { "TIFFLibraryIsNotAvailable", 267 },
    { "XMLLibraryIsNotAvailable", 268 },
    { "XWindowLibraryIsNotAvailable", 269 },
    { "ZipLibraryIsNotAvailable", 270 },
    { "Default", 271 },
    { "Default", 272 },
    { "FailedToCloseModule", 273 },
    { "FailedToFindSymbol", 274 },
    { "UnableToLoadModule", 275 },
    { "UnableToRegisterImageFormat", 276 },
    { "UnrecognizedModule", 277 },
    { "UnableToInitializeModuleLoader", 278 },
    { "Default", 279 },
    { "Default", 280 },
    { "Default", 281 },
    { "UserRequestedTerminationBySignal", 282 },
    { "Default", 283 },
    { "BevelWidthIsNegative", 284 },
    { "ColorSeparatedImageRequired", 285 },
    { "FrameIsLessThanImageSize", 286 },
    { "GeometryDimensionsAreZero", 287 },
    { "GeometryDoesNotContainImage", 288 },
    { "HaldClutImageDimensionsInvalid", 289 },
    { "ImagesAreNotTheSameSize", 290 },
    { "ImageSizeMustExceedBevelWidth", 291 },
    { "ImageSmallerThanKernelWidth", 292 },
    { "ImageSmallerThanRadius", 293 },
    { "ImageWidthsOrHeightsDiffer", 294 },
    { "InputImagesAlreadySpecified", 295 },
    { "InvalidSubimageSpecification", 296 },
    { "KernelRadiusIsTooSmall", 297 },
    { "KernelWidthMustBeAnOddNumber", 298 },
    { "MatrixIsNotSquare", 299 },
    { "MatrixOrderOutOfRange", 300 },
    { "MissingAnImageFilename", 301 },
    { "MissingArgument", 302 },
    { "MustSpecifyAnImageName", 303 },
    { "MustSpecifyImageSize", 304 },
    { "NoBlobDefined", 305 },
    { "NoImagesDefined", 306 },
    { "NonzeroWidthAndHeightRequired", 307 },
    { "NoProfileNameWasGiven", 308 },
    { "NullBlobArgument", 309 },
    { "ReferenceImageRequired", 310 },
    { "ReferenceIsNotMyType", 311 },
    { "RegionAreaExceedsLimit", 312 },
    { "RequestDidNotReturnAnImage", 313 },
    { "SteganoImageRequired", 314 },
    { "StereoImageRequired", 315 },
    { "SubimageSpecificationReturnsNoImages", 316 },
    { "TileNotBoundedByImageDimensions", 317 },
    { "UnableToAddOrRemoveProfile", 318 },
    { "UnableToAverageImageSequence", 319 },
    { "UnableToBlurImage", 320 },
    { "UnableToChopImage", 321 },
    { "UnableToColorMatrixImage", 322 },
    { "UnableToConstituteImage", 323 },
    { "UnableToConvolveImage", 324 },
    { "UnableToEdgeImage", 325 },
    { "UnableToEqualizeImage", 326 },
    { "UnableToFilterImage", 327 },
    { "UnableToFormatImageMetadata", 328 },
    { "UnableToFrameImage", 329 },
    { "UnableToOilPaintImage", 330 },
    { "UnableToPaintImage", 331 },
    { "UnableToRaiseImage", 332 },
    { "UnableToSharpenImage", 333 },
    { "UnableToThresholdImage", 334 },
    { "UnableToWaveImage", 335 },
    { "UnrecognizedAttribute", 336 },
    { "UnrecognizedChannelType", 337 },
    { "UnrecognizedColor", 338 },
    { "UnrecognizedColormapType", 339 },
    { "UnrecognizedColorspace", 340 },
    { "UnrecognizedCommand", 341 },
    { "UnrecognizedComposeOperator", 342 },
    { "UnrecognizedDisposeMethod", 343 },
    { "UnrecognizedElement", 344 },
    { "UnrecognizedEndianType", 345 },
    { "UnrecognizedGravityType", 346 },
    { "UnrecognizedHighlightStyle", 347 },
    { "UnrecognizedImageCompression", 348 },
    { "UnrecognizedImageFilter", 349 },
    { "UnrecognizedImageFormat", 350 },
    { "UnrecognizedImageMode", 351 },
    { "UnrecognizedImageType", 352 },
    { "UnrecognizedIntentType", 353 },
    { "UnrecognizedInterlaceType", 354 },
    { "UnrecognizedListType", 355 },
    { "UnrecognizedMetric", 356 },
    { "UnrecognizedModeType", 357 },
    { "UnrecognizedNoiseType", 358 },
    { "UnrecognizedOperator", 359 },
    { "UnrecognizedOption", 360 },
    { "UnrecognizedPerlMagickMethod", 361 },
    { "UnrecognizedPixelMap", 362 },
    { "UnrecognizedPreviewType", 363 },
    { "UnrecognizedResourceType", 364 },
    { "UnrecognizedType", 365 },
    { "UnrecognizedUnitsType", 366 },
    { "UnrecognizedVirtualPixelMethod", 367 },
    { "UnsupportedSamplingFactor", 368 },
    { "UsageError", 369 },
    { "InvalidColorspaceType", 370 },
    { "InvalidEndianType", 371 },
    { "InvalidImageType", 372 },
    { "InvalidInterlaceType", 373 },
    { "MissingAnImageFilename", 374 },
    { "MissingArgument", 375 },
    { "NoImagesWereLoaded", 376 },
    { "OptionLengthExceedsLimit", 377 },
    { "RequestDidNotReturnAnImage", 378 },
    { "UnableToOpenXServer", 379 },
    { "UnableToPersistKey", 380 },
    { "UnrecognizedColormapType", 381 },
    { "UnrecognizedColorspaceType", 382 },
    { "UnrecognizedDisposeMethod", 383 },
    { "UnrecognizedEndianType", 384 },
    { "UnrecognizedFilterType", 385 },
    { "UnrecognizedImageCompressionType", 386 },
    { "UnrecognizedImageType", 387 },
    { "UnrecognizedInterlaceType", 388 },
    { "UnrecognizedOption", 389 },
    { "UnrecognizedResourceType", 390 },
    { "UnrecognizedVirtualPixelMethod", 391 },
    { "UnrecognizedColor", 392 },
    { "ImageExpected", 393 },
    { "ImageInfoExpected", 394 },
    { "StructureSizeMismatch", 395 },
    { "UnableToGetRegistryID", 396 },
    { "UnableToLocateImage", 397 },
    { "UnableToSetRegistry", 398 },
    { "Default", 399 },
    { "Default", 400 },
    { "CacheResourcesExhausted", 401 },
    { "ImagePixelHeightLimitExceeded", 402 },
    { "ImagePixelLimitExceeded", 403 },
    { "ImagePixelWidthLimitExceeded", 404 },
    { "MemoryAllocationFailed", 405 },
    { "NoPixelsDefinedInCache", 406 },
    { "PixelCacheAllocationFailed", 407 },
    { "UnableToAddColorProfile", 408 },
    { "UnableToAddGenericProfile", 409 },
    { "UnableToAddIPTCProfile", 410 },
    { "UnableToAddOrRemoveProfile", 411 },
    { "UnableToAllocateCoefficients", 412 },
    { "UnableToAllocateColormap", 413 },
    { "UnableToAllocateICCProfile", 414 },
    { "UnableToAllocateImage", 415 },
    { "UnableToAllocateString", 416 },
    { "UnableToAnnotateImage", 417 },
    { "UnableToAverageImageSequence", 418 },
    { "UnableToCloneDrawingWand", 419 },
    { "UnableToCloneImage", 420 },
    { "UnableToComputeImageSignature", 421 },
    { "UnableToConstituteImage", 422 },
    { "UnableToConvertFont", 423 },
    { "UnableToConvertStringToTokens", 424 },
    { "UnableToCreateColormap", 425 },
    { "UnableToCreateColorTransform", 426 },
    { "UnableToCreateCommandWidget", 427 },
    { "UnableToCreateImageGroup", 428 },
    { "UnableToCreateImageMontage", 429 },
    { "UnableToCreateXWindow", 430 },
    { "UnableToCropImage", 431 },
    { "UnableToDespeckleImage", 432 },
    { "UnableToDetermineImageClass", 433 },
    { "UnableToDetermineTheNumberOfImageColors", 434 },
    { "UnableToDitherImage", 435 },
    { "UnableToDrawOnImage", 436 },
    { "UnableToEdgeImage", 437 },
    { "UnableToEmbossImage", 438 },
    { "UnableToEnhanceImage", 439 },
    { "UnableToFloodfillImage", 440 },
    { "UnableToGammaCorrectImage", 441 },
    { "UnableToGetBestIconSize", 442 },
    { "UnableToGetFromRegistry", 443 },
    { "UnableToGetPackageInfo", 444 },
    { "UnableToLevelImage", 445 },
    { "UnableToMagnifyImage", 446 },
    { "UnableToManageColor", 447 },
    { "UnableToMapImage", 448 },
    { "UnableToMapImageSequence", 449 },
    { "UnableToMedianFilterImage", 450 },
    { "UnableToMotionBlurImage", 451 },
    { "UnableToNoiseFilterImage", 452 },
    { "UnableToNormalizeImage", 453 },
    { "UnableToOpenColorProfile", 454 },
    { "UnableToQuantizeImage", 455 },
    { "UnableToQuantizeImageSequence", 456 },
    { "UnableToReadTextChunk", 457 },
    { "UnableToReadXImage", 458 },
    { "UnableToReadXServerColormap", 459 },
    { "UnableToResizeImage", 460 },
    { "UnableToRotateImage", 461 },
    { "UnableToSampleImage", 462 },
    { "UnableToScaleImage", 463 },
    { "UnableToSelectImage", 464 },
    { "UnableToSharpenImage", 465 },
    { "UnableToShaveImage", 466 },
    { "UnableToShearImage", 467 },
    { "UnableToSortImageColormap", 468 },
    { "UnableToThresholdImage", 469 },
    { "UnableToTransformColorspace", 470 },
    { "MemoryAllocationFailed", 471 },
    { "SemaporeOperationFailed", 472 },
    { "UnableToAllocateAscii85Info", 473 },
    { "UnableToAllocateCacheInfo", 474 },
    { "UnableToAllocateCacheView", 475 },
    { "UnableToAllocateColorInfo", 476 },
    { "UnableToAllocateDashPattern", 477 },
    { "UnableToAllocateDelegateInfo", 478 },
    { "UnableToAllocateDerivatives", 479 },
    { "UnableToAllocateDrawContext", 480 },
    { "UnableToAllocateDrawInfo", 481 },
    { "UnableToAllocateDrawingWand", 482 },
    { "UnableToAllocateGammaMap", 483 },
    { "UnableToAllocateImage", 484 },
    { "UnableToAllocateImagePixels", 485 },
    { "UnableToAllocateLogInfo", 486 },
    { "UnableToAllocateMagicInfo", 487 },
    { "UnableToAllocateMagickInfo", 488 },
    { "UnableToAllocateMagickMap", 489 },
    { "UnableToAllocateModuleInfo", 490 },
    { "UnableToAllocateMontageInfo", 491 },
    { "UnableToAllocateQuantizeInfo", 492 },
    { "UnableToAllocateRandomKernel", 493 },
    { "UnableToAllocateRegistryInfo", 494 },
    { "UnableToAllocateSemaphoreInfo", 495 },
    { "UnableToAllocateString", 496 },
    { "UnableToAllocateTypeInfo", 497 },
    { "UnableToAllocateWand", 498 },
    { "UnableToAnimateImageSequence", 499 },
    { "UnableToCloneBlobInfo", 500 },
    { "UnableToCloneCacheInfo", 501 },
    { "UnableToCloneImage", 502 },
    { "UnableToCloneImageInfo", 503 },
    { "UnableToConcatenateString", 504 },
    { "UnableToConvertText", 505 },
    { "UnableToCreateColormap", 506 },
    { "UnableToDestroySemaphore", 507 },
    { "UnableToDisplayImage", 508 },
    { "UnableToEscapeString", 509 },
    { "UnableToInitializeSemaphore", 510 },
    { "UnableToInterpretMSLImage", 511 },
    { "UnableToLockSemaphore", 512 },
    { "UnableToObtainRandomEntropy", 513 },
    { "UnableToUnlockSemaphore", 514 },
    { "MemoryAllocationFailed", 515 },
    { "ImageDoesNotContainTheStreamGeometry", 516 },
    { "NoStreamHandlerIsDefined", 517 },
    { "PixelCacheIsNotOpen", 518 },
    { "UnableToAcquirePixelStream", 519 },
    { "UnableToSetPixelStream", 520 },
    { "UnableToSyncPixelStream", 521 },
    { "Default", 522 },
    { "Default", 523 },
    { "FontNotSpecified", 524 },
    { "FontSubstitutionRequired", 525 },
    { "UnableToGetTypeMetrics", 526 },
    { "UnableToInitializeFreetypeLibrary", 527 },
    { "UnableToReadFont", 528 },
    { "UnrecognizedFontEncoding", 529 },
    { "Default", 530 },
    { "Default", 531 },
    { "InvalidColormapIndex", 532 },
    { "WandAPINotImplemented", 533 },
    { "WandContainsNoImageIndexs", 534 },
    { "WandContainsNoImages", 535 },
    { "ColorIsNotKnownToServer", 536 },
    { "NoWindowWithSpecifiedIDExists", 537 },
    { "StandardColormapIsNotInitialized", 538 },
    { "UnableToConnectToRemoteDisplay", 539 },
    { "UnableToCreateBitmap", 540 },
    { "UnableToCreateColormap", 541 },
    { "UnableToCreatePixmap", 542 },
    { "UnableToCreateProperty", 543 },
    { "UnableToCreateStandardColormap", 544 },
    { "UnableToDisplayImageInfo", 545 },
    { "UnableToGetProperty", 546 },
    { "UnableToGetStandardColormap", 547 },
    { "UnableToGetVisual", 548 },
    { "UnableToGrabMouse", 549 },
    { "UnableToLoadFont", 550 },
    { "UnableToMatchVisualToStandardColormap", 551 },
    { "UnableToOpenXServer", 552 },
    { "UnableToReadXAttributes", 553 },
    { "UnableToReadXWindowImage", 554 },
    { "UnrecognizedColormapType", 555 },
    { "UnrecognizedGravityType", 556 },
    { "UnrecognizedVisualSpecifier", 557 },
    { "UnableToAllocateXHints", 558 },
    { "UnableToCreateCursor", 559 },
    { "UnableToCreateGraphicContext", 560 },
    { "UnableToCreateStandardColormap", 561 },
    { "UnableToCreateTextProperty", 562 },
    { "UnableToCreateXImage", 563 },
    { "UnableToCreateXPixmap", 564 },
    { "UnableToCreateXWindow", 565 },
    { "UnableToDisplayImage", 566 },
    { "UnableToDitherImage", 567 },
    { "UnableToGetPixelInfo", 568 },
    { "UnableToGetVisual", 569 },
    { "UnableToLoadFont", 570 },
    { "UnableToMakeXWindow", 571 },
    { "UnableToOpenXServer", 572 },
    { "UnableToViewFonts", 573 },
    { "UnableToGetVisual", 574 },
    { "UsingDefaultVisual", 575 },
    { 0, 0 }
  };
#endif

#if defined(_INCLUDE_MESSAGE_TABLE_)
static const char *message_dat[] =
  {
    "%1",
    "Unable to create blob",
    "Unable to deduce image format",
    "Unable to obtain current offset",
    "Unable to open file",
    "Unable to read file",
    "Unable to read to offset",
    "Unable to seek to offset",
    "Unable to write blob",
    "Unrecognized image format",
    "default error",
    "default warning",
    "Inconsistent persistent cache depth",
    "Pixel cache dimensions incompatible with image dimensions",
    "Pixel cache is not open",
    "Unable to allocate cache view",
    "Unable to clone cache",
    "Unable to extend cache",
    "Unable to get cache nexus",
    "Unable to get pixels from cache",
    "Unable to open cache",
    "Unable to persist pixel cache",
    "Unable to read pixel cache",
    "Unable to sync cache (check temporary file disk space)",
    "disk allocation failed",
    "Unable to extend pixel cache",
    "default warning",
    "Arithmetic overflow",
    "Colormap size exceeds limit",
    "Colormap type not supported",
    "Colorspace model is not supported",
    "Color type not supported",
    "Compression not valid",
    "Data encoding scheme is not supported",
    "Data storage type is not supported",
    "Coder did not return an image (this is a bug, please report it!)",
    "Delta-PNG is not supported",
    "Division by zero",
    "Encrypted WPG image file not supported",
    "Fractal compression not supported",
    "Image column or row size is not supported",
    "Image does not have a matte channel",
    "Image is not tiles",
    "Image type not supported",
    "Incompatible size of double",
    "Irregular channel geometry not supported",
    "JNG compression is not supported",
    "JPEG compression is not supported",
    "JPEG embedding failed",
    "Location type is not supported",
    "Map storage type is not supported",
    "MSB order not supported bitmap",
    "Multi-dimensional matrices are not supported",
    "Multiple record list not supported",
    "No 8BIM data is available",
    "No APP1 data is available",
    "No bitmap on clipboard",
    "No color profile available",
    "No data returned",
    "No image vector graphics",
    "No IPTC info was found",
    "No IPTC profile available",
    "Number of images is not supported",
    "Only continuous tone picture supported",
    "Only level zero files Supported",
    "PNG compression is not supported",
    "PNG library is too old",
    "RLE compression not supported",
    "Subsampling requires that image width be evenly divisible by two",
    "Unable to copy profile",
    "Unable to create a DC",
    "Unable to create bitmap",
    "Unable to decompress image",
    "Unable to Initialize FPX library",
    "Unable to open blob",
    "Unable to read aspect ratio",
    "Unable to read CIELAB images",
    "Unable to read summary info",
    "Unable to set affine matrix",
    "Unable to set aspect ratio",
    "Unable to set color twist",
    "Unable to set contrast",
    "Unable to set filtering value",
    "Unable to set image comment",
    "Unable to set image title",
    "Unable to set JPEG level",
    "Unable to set region of interest",
    "Unable to set summary info",
    "Unable to translate text",
    "Unable to write MPEG parameters",
    "Unable to write to temporary file",
    "Unable to zip-compress image",
    "Unsupported bits per sample",
    "Unsupported cell type in the matrix",
    "Unsupported samples per pixel",
    "WebP decoding failed: user abort",
    "WebP encoding failed: unknown reason",
    "WebP encoding failed: bad dimension",
    "WebP encoding failed: bad write",
    "WebP encoding failed: bitstream out of memory",
    "WebP encoding failed: File too big (> 4GB)",
    "WebP encoding failed: invalid configuration",
    "WebP encoding failed: null parameter",
    "WebP encoding failed: out of memory",
    "WebP encoding failed: partition 0 overflow (> 512K)",
    "WebP encoding failed: partition overflow (> 16M)",
    "WebP encoding failed: user abort",
    "Invalid WebP configuration parameters supplied",
    "WebP failed: invalid parameter",
    "ZIP compression is not supported",
    "default error",
    "Lossless to lossy JPEG conversion",
    "include element nested too deeply",
    "Registry key lookup failed. Package is not properly installed on this machine.",
    "String token maximum length exceeded",
    "Unable to access configuration file",
    "Unable to access font file",
    "Unable to access log configuration file",
    "Unable to access module file",
    "default error",
    "Unable to change to working directory",
    "Unable to get current working directory",
    "Unable to restore current working directory",
    "default warning",
    "An error has occurred reading from file",
    "An error has occurred writing to file",
    "Colormap exceeded colors limit",
    "Compression not valid",
    "Corrupt image",
    "Image file or blob does not contain any image data",
    "Image file has no scenes",
    "Image type not supported",
    "Improper image header",
    "Insufficient image data in file",
    "Invalid colormap index",
    "invalid file format version",
    "Length and filesize do not match",
    "Missing a required image channel",
    "Negative or zero image size",
    "Non OS2 BMP header size less than 40",
    "Not enough tiles found in level",
    "Static planes value not equal to 1",
    "Subsampling requires that image width be evenly divisible by two",
    "Too much image data in file",
    "Unable to read colormap from dump file",
    "Unable to read color profile",
    "Unable to read extension block",
    "Unable to read generic profile",
    "Unable to read image data",
    "Unable to read image header",
    "Unable to read IPTC profile",
    "Unable to read pixmap from dump file",
    "Unable to read sub image data",
    "Unable to read VID image",
    "Unable to read window name from dump file",
    "Unable to runlength decode image",
    "Unable to uncompress image",
    "Unexpected end-of-file",
    "Unexpected sampling factor",
    "Unknown pattern type",
    "Unrecognized bits per pixel",
    "Unrecognized compression",
    "Unrecognized number of colors",
    "Unrecognized XWD header",
    "Unsupported bits per sample",
    "Unsupported number of planes",
    "Unable to persist key",
    "Compression not valid",
    "Corrupt image (some data returned)",
    "Improper image header",
    "Invalid colormap index",
    "Length and filesize do not match",
    "Negative or zero image size",
    "Non OS2 header size error",
    "Corrupt PCD image, skipping to sync byte",
    "Static planes value not equal to one",
    "Unable to parse embedded profile",
    "Unrecognized bits per pixel",
    "Unrecognized image compression",
    "Delegate failed",
    "Failed to allocate argument list.",
    "Failed to allocate Ghostscript interpreter.",
    "Failed to compute output size",
    "Failed to find Ghostscript (not installed?).",
    "Failed to render file",
    "Failed to scan file",
    "No tag found",
    "Postscript delegate failed",
    "Unable to create image",
    "Unable to create image component",
    "Unable to decode image file",
    "Unable to encode image file",
    "Unable to initialize FPX library",
    "Unable to initialize WMF library",
    "Unable to manage JP2 stream",
    "Unable to write SVG format",
    "WebP library ABI does not match header ABI (build issue!)",
    "default error",
    "default warning",
    "Already pushing pattern definition",
    "drawing recursion detected",
    "text value does not convert to float",
    "text value does not convert to integer",
    "invalid primitive argument",
    "Non-conforming drawing primitive definition",
    "primitive arithmetic overflow",
    "too many coordinates",
    "Unable to print",
    "unbalanced graphic context push-pop",
    "unreasonable gradient image size",
    "vector path truncated",
    "default error",
    "Not a relative URL",
    "Not currently pushing pattern definition",
    "URL not found",
    "Unable to create temporary file",
    "Unable to open file",
    "Unable to write file",
    "default error",
    "default warning",
    "angle is discontinuous",
    "CMYKA image lacks an alpha channel (indexes)",
    "Colorspace color profile mismatch",
    "image colorspace differs",
    "image colorspace mismatch",
    "image difference exceeds limit (%s)",
    "image does not contain resolution",
    "image is not colormapped",
    "image opacity differs",
    "Image sequence is required",
    "image size differs",
    "Invalid colormap index",
    "left and right image sizes differ",
    "no images were found",
    "no images were loaded",
    "no [LOCALE] image attribute",
    "too many cluster",
    "unable to append image",
    "Unable to assign profile",
    "unable to average image",
    "unable to coalesce image",
    "unable to compare images",
    "unable to create image mosaic",
    "unable to create stereo image",
    "unable to deconstruct image sequence",
    "unable to export image pixels",
    "unable to flatten image",
    "Unable to get clip mask",
    "Unable to get composite mask",
    "unable to handle image channel",
    "unable to import image pixels",
    "unable to resize image",
    "unable to segment image",
    "Unable to set clip mask",
    "Unable to set composite mask",
    "unable to shear image",
    "width or height exceeds limit",
    "Unable to persist key",
    "default warning",
    "DPS library is not available",
    "FPX library is not available",
    "FreeType library is not available",
    "JPEG compression library is not available",
    "LCMS encoding not enabled",
    "LZW encoding not enabled",
    "No decode delegate for this image format",
    "No encode delegate for this image format",
    "TIFF library is not available",
    "XML library is not available",
    "X Window library is not available",
    "ZLIB compression library is not available",
    "default error",
    "default warning",
    "Failed to close module",
    "Failed to find symbol",
    "Unable to load module",
    "Unable to register image format",
    "Unrecognized module",
    "Unable to initialize module loader",
    "default warning",
    "default error",
    "default error",
    "User requested termination (via signal)",
    "default warning",
    "bevel width is negative",
    "color separated image required",
    "frame is less than image size",
    "geometry dimensions are zero",
    "geometry does not contain image",
    "hald clut image dimensions are invalid",
    "images are not the same size",
    "size must exceed bevel width",
    "image smaller than kernel width",
    "image smaller than radius",
    "image widths or heights differ",
    "input images already specified",
    "Invalid subimage specification",
    "kernel radius is too small",
    "kernel width must be an odd number",
    "Matrix is not square (%s elements)",
    "Matrix size is out of range",
    "Missing an image filename",
    "Option '%s' requires an argument or argument is malformed",
    "Must specify a image name",
    "Must specify image size",
    "No Binary Large OBjects defined",
    "No images defined",
    "Non-zero width and height required",
    "No profile name was given",
    "Null blob argument",
    "Reference image required",
    "Reference is not my type",
    "Region area exceeds implementation limit",
    "Request did not return an image",
    "Stegano image required",
    "Stereo image required",
    "Subimage specification returns no images",
    "Tile is not bounded by image dimensions",
    "Unable to add or remove profile",
    "unable to average image sequence",
    "unable to blur image",
    "unable to chop image",
    "Unable to color matrix image",
    "Unable to constitute image",
    "Unable to convolve image",
    "Unable to edge image",
    "Unable to equalize image",
    "Unable to filter image",
    "unable to format image meta data",
    "Unable to frame image",
    "unable to oil paint image",
    "Unable to paint image",
    "Unable to raise image",
    "Unable to sharpen image",
    "Unable to threshold image",
    "Unable to wave image",
    "Unrecognized attribute",
    "Unrecognized channel type",
    "Unrecognized color",
    "Unrecognized colormap type",
    "Unrecognized image colorspace",
    "Unrecognized command '%s'. Use -help for a usage summary or see manual.",
    "Unrecognized compose operator",
    "Unrecognized dispose method",
    "Unrecognized element",
    "Unrecognized endian type",
    "Unrecognized gravity type",
    "Unrecognized highlight style",
    "Unrecognized image compression",
    "Unrecognized image filter",
    "Unrecognized image format",
    "Unrecognized image mode",
    "Unrecognized image type",
    "Unrecognized intent type",
    "Unrecognized interlace type",
    "Unrecognized list type",
    "Unrecognized error metric",
    "Unrecognized mode type",
    "Unrecognized noise type",
    "Unrecognized operator",
    "Unrecognized option",
    "Unrecognized PerlMagick method",
    "Unrecognized pixel map",
    "Unrecognized preview type",
    "Unrecognized resource type",
    "Unrecognized type",
    "Unrecognized units type",
    "Unrecognized virtual pixel method",
    "Unsupported sampling factor",
    "Improper arguments supplied, please see manual",
    "Invalid colorspace type",
    "Invalid endian type",
    "Invalid image type",
    "Invalid interlace type",
    "Missing an image filename",
    "Option '%s' requires an argument or argument is malformed",
    "No images were loaded",
    "Option length exceeds limit",
    "Request did not return an image",
    "Unable to open XServer",
    "Unable to persist key",
    "Unrecognized colormap type",
    "Unrecognized colorspace type",
    "unrecognized dispose method",
    "Unrecognized endian type",
    "Unrecognized filter type",
    "unrecognized compression type",
    "Unrecognized image type",
    "Unrecognized interlace type",
    "Unrecognized option",
    "Unrecognized resource type",
    "Unrecognized virtual pixel method",
    "Unrecognized color",
    "image expected",
    "image info expected",
    "structure size mismatch",
    "Unable to get registry ID",
    "Unable to locate image",
    "Unable to set registry",
    "default error",
    "default warning",
    "Disk space limit exceeded (see -limit Disk)",
    "Image pixel height limit exceeded (see -limit Height)",
    "Image pixel limit exceeded (see -limit Pixels)",
    "Image pixel width limit exceeded (see -limit Width)",
    "Memory allocation failed",
    "No pixels defined in cache",
    "Pixel cache allocation failed",
    "unable to add ICC Color profile",
    "unable to add generic profile",
    "unable to add IPTC profile",
    "unable to add or remove profile",
    "unable to allocate coefficients",
    "Unable to allocate colormap",
    "unable to allocate ICC profile",
    "Unable to allocate image",
    "unable to allocate string",
    "Unable to annotate image",
    "unable to average image sequence",
    "unable to clone drawing wand",
    "unable to clone image",
    "unable to compute image signature",
    "unable to constitute image",
    "unable to convert font",
    "unable to convert strings to tokens",
    "Unable to create colormap",
    "unable to create color transform",
    "unable to create command widget",
    "unable to create image group",
    "Unable to create image montage",
    "unable to create X window",
    "unable to crop image",
    "unable to despeckle image",
    "unable to determine image class",
    "unable to determine the number of image colors",
    "unable to dither image",
    "unable to draw on image",
    "unable to edge image",
    "unable to emboss image",
    "unable to enhance image",
    "unable to floodfill image",
    "unable to gamma correct image",
    "unable to get best icon size",
    "unable to get from registry",
    "Unable to get package info",
    "unable to level image",
    "unable to magnify image",
    "Unable to manage color",
    "Unable to map image",
    "Unable to map image sequence",
    "unable to median filter image",
    "unable to motion blur image",
    "unable to noise filter image",
    "unable to normalize image",
    "unable to open color profile",
    "unable to quantize image",
    "unable to quantize image sequence",
    "unable to read text chunk",
    "unable to read X image",
    "unable to read X server colormap",
    "unable to resize image",
    "unable to rotate image",
    "unable to sample image",
    "unable to scale image",
    "unable to select image",
    "unable to sharpen image",
    "unable to shave image",
    "unable to shear image",
    "unable to sort image colormap",
    "unable to threshold image",
    "unable to transform colorspace",
    "Memory allocation failed",
    "Semaphore operation failed",
    "unable to allocate ascii85 info",
    "unable to allocate cache info",
    "unable to allocate cache view",
    "unable to allocate color info",
    "unable to allocate dash pattern",
    "unable to allocate delegate info",
    "unable to allocate derivates",
    "unable to allocate draw context",
    "unable to allocate draw info",
    "unable to allocate drawing wand",
    "unable to allocate gamma map",
    "unable to allocate image",
    "unable to allocate image pixels",
    "unable to allocate log info",
    "unable to allocate magic info",
    "unable to allocate magick info",
    "unable to allocate magick map",
    "unable to allocate module info",
    "unable to allocate montage info",
    "unable to allocate quantize info",
    "unable to allocate random kernel",
    "unable to allocate registry info",
    "unable to allocate semaphore info",
    "unable to allocate string",
    "unable to allocate type info",
    "unable to allocate wand",
    "unable to animate image sequence",
    "unable to clone blob info",
    "unable to clone cache info",
    "unable to clone image",
    "unable to clone image info",
    "unable to concatenate string",
    "unable to convert text",
    "unable to create colormap",
    "unable to destroy semaphore",
    "unable to display image",
    "unable to escape string",
    "unable to initialize semaphore",
    "unable to interpret MSL image",
    "unable to lock semaphore",
    "unable to obtain random bytes from operating system",
    "unable to unlock semaphore",
    "Memory allocation failed",
    "image does not contain the stream geometry",
    "no stream handler is defined",
    "Pixel cache is not open",
    "Unable to acquire pixel stream",
    "Unable to set pixel stream",
    "Unable to sync pixel stream",
    "default error",
    "default warning",
    "Font name not specified",
    "Font substitution required",
    "Unable to get type metrics",
    "Unable to initialize freetype library",
    "Unable to read font",
    "Unrecognized font encoding",
    "default error",
    "default warning",
    "invalid colormap index `%.1024s",
    "Wand API not implemented `%.1024s",
    "Wand contains no image indices `%.1024s",
    "Wand contains no images `%.1024s",
    "Color is not known to server",
    "No window with specified ID exists",
    "Standard Colormap is not initialized",
    "Unable to connect to remote display",
    "Unable to create bitmap",
    "Unable to create colormap",
    "Unable to create pixmap",
    "Unable to create property",
    "Unable to create standard colormap",
    "Unable to display image info",
    "Unable to get property",
    "Unable to get Standard Colormap",
    "Unable to get visual",
    "Unable to grab mouse",
    "Unable to load font",
    "Unable to match visual to Standard Colormap",
    "Unable to open X server",
    "Unable to read X attributes",
    "Unable to read X window image",
    "Unrecognized colormap type",
    "Unrecognized gravity type",
    "Unrecognized visual specifier",
    "Unable to allocate X hints",
    "Unable to create X cursor",
    "Unable to create graphic context",
    "unable to create standard colormap",
    "Unable to create text property",
    "Unable to create X image",
    "Unable to create X pixmap",
    "Unable to create X window",
    "unable to display image",
    "unable to dither image",
    "Unable to get pixel info",
    "Unable to get visual",
    "Unable to load font",
    "Unable to make X window",
    "Unable to open X server",
    "Unable to view fonts",
    "Unable to get visual",
    "UsingDefaultVisual",
    0
  };
#endif
