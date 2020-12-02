//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file MyDetectorConstruction.cc
/// \brief Implementation of the MyDetectorConstruction class

#include "MyDetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnionSolid.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyDetectorConstruction::MyDetectorConstruction()
: G4VUserDetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyDetectorConstruction::~MyDetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* MyDetectorConstruction::Construct()
{  
  // Get nist material manager
  G4NistManager* nistManager = G4NistManager::Instance();

  // Build materials

  // Vacuum

  G4double atomicNumber = 1.;
  G4double massOfMole = 1.008*g/mole;
	G4double density = 1.e-25*mg/cm3; 
	G4double temperature = 2.73*kelvin;
	G4double pressure = 1.e-18*pascal;

	G4Material* vacuum = new G4Material("Vacuum", atomicNumber, massOfMole, density,  kStateGas, temperature, pressure);

  G4Material* air = nistManager->FindOrBuildMaterial("G4_AIR");
  G4Material* steel = nistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL");
	G4Material* silicon = nistManager->FindOrBuildMaterial("G4_Si");
       // There is no need to test if materials were built/found
       // as G4NistManager would issue an error otherwise
       // Try the code with "XYZ".      

  // Print all materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;         
  
  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;

  //     
  // World
  //
  G4double hx = 5.*cm;
  G4double hy = 5.*cm;
  G4double hz = 5.*cm;

  // world volume
  G4Box* worldS = new G4Box("World", hx, hy, hz); 
      
  G4LogicalVolume* worldLV                         
    = new G4LogicalVolume(worldS, air, "World");
                                   
  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      worldLV,               //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  // Volume diode "pleine"
  G4double rmin = 0.;
	G4double rmax = 17*mm;
  G4double hdiode = 5.*mm; //1cm de haut
	G4double phimin = 0.;
  G4double dphi = 360.*deg;

	// diode pleine volume
  G4VSolid* diodepleineS = new G4Tubs("DiodePleine", rmin, rmax, hdiode, phimin, dphi);

	G4LogicalVolume* diodepleineLV
    = new G4LogicalVolume(diodepleineS, steel, "DiodePleine");

  new G4PVPlacement(0, 
                    G4ThreeVector(),       //at (0,0,0)
                    diodepleineLV,                //its logical volume
                    "DiodePleine",                //its name
                    worldLV,               //its mother  volume
                    false,                 //no boolean operation
                    0,                     //copy number
                    checkOverlaps);        //overlaps checking

	// Volume diode entrée vide
  rmin = 13.82*mm;
	rmax = 17*mm;
	G4double hvide = 500.*um; //1mm de vide, comblé après par 100um de surface de détection
	phimin = 0.;
	dphi = 360.*deg;

  // diode entrée volume
	G4VSolid* entreeS = new G4Tubs("Entree", rmin, rmax, hvide, phimin, dphi);
  
  G4LogicalVolume* entreeLV
    = new G4LogicalVolume(entreeS, steel, "Entree");

  new G4PVPlacement(0, 
                    G4ThreeVector(0., 0., hdiode+hvide),       //at (0,0,0)
                    entreeLV,                //its logical volume
                    "Entree",                //its name
                    worldLV,               //its mother  volume
                    false,                 //no boolean operation
                    0,                     //copy number
                    checkOverlaps);        //overlaps checking
	// Volume diode sensible
	rmin = 0.;
	rmax = 13.82*mm;
	G4double hsensible = 50.*um; //100um de profondeur sensible
	phimin = 0.;
	dphi = 360.*deg;

  // diode sensible volume
	G4VSolid* cibleS = new G4Tubs("Cible", rmin, rmax, hsensible, phimin, dphi);
  
  G4LogicalVolume* cibleLV
    = new G4LogicalVolume(cibleS, silicon, "Cible");

  new G4PVPlacement(0, 
                    G4ThreeVector(0,0,hdiode+hsensible),
                    cibleLV,                //its logical volume
                    "Cible",                //its name
                    worldLV,               //its mother  volume
                    false,                 //no boolean operation
                    0,                     //copy number
                    checkOverlaps);        //overlaps checking

	

  // Volume butée diode
  rmin = 16.*mm;
	rmax = 18.*mm;
  G4double hbutee = 0.75*mm; // butée de 1.5mm
	phimin = 0.;
  dphi = 360.*deg;

  // butée volume

	G4VSolid* buteeS = new G4Tubs("Butee", rmin, rmax, hbutee, phimin, dphi);
  
  G4LogicalVolume* buteeLV
    = new G4LogicalVolume(buteeS, steel, "Butee");

  new G4PVPlacement(0, 
                    G4ThreeVector(0,0,hdiode+2.*hvide+hbutee),
                    buteeLV,                //its logical volume
                    "Butee",                //its name
                    worldLV,               //its mother  volume
                    false,                 //no boolean operation
                    0,                     //copy number
                    checkOverlaps);        //overlaps checking
  
	// Volume source
	rmin = 0.;
	rmax = 3.5*mm; // diam 7mm
	G4double hgap = 1.*mm; //source non collée à la fenêtre d'entrée
	G4double hsource = 0.25*mm; // source d'épaisseur 0.5mm
	G4double hdelta = 0.2*mm; // petit delta pour ne pas superposer la source réelle et sa modélisation
	phimin = 0.;
	dphi = 360.*deg;

	// source volume

	G4VSolid* sourceS = new G4Tubs("Source", rmin, rmax, hsource, phimin, dphi);
  
  G4LogicalVolume* sourceLV
    = new G4LogicalVolume(sourceS, steel, "Source");

  new G4PVPlacement(0, 
                    G4ThreeVector(0,0,hdiode+2.*hvide+2.*hbutee+hsource+hgap+hdelta),
                    sourceLV,                //its logical volume
                    "Source",                //its name
                    worldLV,               //its mother  volume
                    false,                 //no boolean operation
                    0,                     //copy number
                    checkOverlaps);        //overlaps checking  

  //always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
