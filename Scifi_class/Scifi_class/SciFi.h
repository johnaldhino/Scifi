#ifndef ROOT_SciFi
#define ROOT_SciFi

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// SciFi                                                              //
//                                                                           //
// Generic Cherenkov detector.                                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaPidDetector.h"
#include "Fadc250Module.h"

class TClonesArray;

class SciFi : public THaPidDetector {

public:
  SciFi( const char* name, const char* description = "",
	      THaApparatus* a = NULL );
  SciFi(); // for ROOT I/O
  virtual ~SciFi();

  virtual void       Clear( Option_t* ="" );
  virtual Int_t      Decode( const THaEvData& );
  virtual Int_t      CoarseProcess( TClonesArray& tracks );
  virtual Int_t      FineProcess( TClonesArray& tracks );
          Float_t    GetAsum() const { return fASUM_c; }

protected:

  // Calibration
  Float_t*   fOff;        // [fNelem] TDC offsets (chan)
  Float_t*   fPed;        // [fNelem] ADC pedestals (chan)
  Float_t*   fGain;       // [fNelem] ADC gains

  Int_t    fNPED;        //number of samples included in FADC pedestal sum
  Int_t    fNSA;         //number of integration samples after threshold crossing
  Int_t    fNSB;         //number of integration samples before threshold crossing
  Int_t    fWin;         //total number of samples in FADC window
  Int_t    fTFlag;       //flag for FADC threshold on vs FADC threshold off


  // Per-event data
  Int_t      fNThit;      // Number of mirrors with non zero TDC times
  Float_t*   fT;          // [fNelem] Array of TDC times of channels
  Float_t*   fT_c;        // [fNelem] Array of corrected TDC times of channels
  Int_t      fNAhit;      // Number of mirrors with non zero ADC amplitudes
  Float_t*   fA;          // [fNelem] Array of ADC amplitudes of channels
  Float_t*   fA_p;        // [fNelem] Array of ADC minus pedestal values of chans
  Float_t*   fA_c;        // [fNelem] Array of corrected ADC amplitudes of chans
  Float_t    fASUM_p;     // Sum of ADC minus pedestal values of channels
  Float_t    fASUM_c;     // Sum of corrected ADC amplitudes of channels

  //FADC
  Float_t*   fPeak;         // [fNelem] Array of FADC ADC peak values
  Float_t*   fT_FADC;       // [fNelem] Array of FADC TDC times of channels
  Float_t*   fT_FADC_c;     // [fNelem] Array of FADC corrected TDC times of channels
  Int_t* foverflow;         //[fNelem] FADC overflowbit
  Int_t* funderflow;        //[fNelem] FADC underflowbit
  Int_t* fpedq;             //[fNelem] FADC pedestal quality bit
  Decoder::Fadc250Module *fFADC;     //pointer to FADC250Module class

  Int_t* fNhits;           //[fNelem] number of hits for each PMT


  virtual Int_t  DefineVariables( EMode mode = kDefine );
          void   DeleteArrays();
  virtual Int_t  ReadDatabase( const TDatime& date );

  ClassDef(SciFi,0)    //Generic Cherenkov class
};

////////////////////////////////////////////////////////////////////////////////

#endif
