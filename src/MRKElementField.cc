#include "G4GeometryManager.hh"

#include "MRKElementField.hh"

#include "MRKGlobalField.hh"

G4Navigator* MRKElementField::aNavigator;

MRKElementField::MRKElementField(G4ThreeVector c, G4LogicalVolume* lv)
{
	center = c;

	minX = minY = minZ = -DBL_MAX;
	maxX = maxY = maxZ = DBL_MAX;

	MRKGlobalField::getObject()->addElementField(this);

	color = "1,1,1";

	userLimits = new G4UserLimits();

	lvolume = lv;
	lvolume->SetVisAttributes(getVisAttribute(color));

	maxStep = 1 * CLHEP::um;

	userLimits->SetMaxAllowedStep(maxStep);

	userLimits->SetUserMaxTime(PROTON_MAX_TIME * CLHEP::second);
	userLimits->SetUserMinEkine(1 * CLHEP::keV);
	userLimits->SetUserMinRange(1 * CLHEP::um);

	lvolume->SetUserLimits(userLimits);
}

void MRKElementField::construct()
{
	G4Navigator* theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

	if(!aNavigator)
	{
		aNavigator = new G4Navigator();
		if(theNavigator->GetWorldVolume()) aNavigator->SetWorldVolume(theNavigator->GetWorldVolume());
	}

	G4GeometryManager* geomManager = G4GeometryManager::GetInstance();

	if(!geomManager->IsGeometryClosed())
	{
		geomManager->OpenGeometry();
		geomManager->CloseGeometry(true);
	}

	aNavigator->LocateGlobalPointAndSetup(center, 0, false);

	G4TouchableHistoryHandle fTouchable = aNavigator->CreateTouchableHistoryHandle();

	G4int depth = fTouchable->GetHistoryDepth();
	for (G4int i = 0; i < depth; ++i)
	{
		if(fTouchable->GetVolume()->GetLogicalVolume() == lvolume) break;
		fTouchable->MoveUpHistory();
	}

	// set global2local transform
	global2local = fTouchable->GetHistory()->GetTopTransform();

	// set global bounding box
	G4double local[4], global[4];

	G4ThreeVector globalPosition;
	local[3] = 0.0;
	for (int i = 0; i < 2; ++i)
	{
		local[0] = (i == 0 ? -1.0 : 1.0) * getWidth() / 2.;
		for (int j = 0; j < 2; ++j)
		{
			local[1] = (j == 0 ? -1.0 : 1.0) * getHeight() / 2.;
			for (int k = 0; k < 2; ++k)
			{
				local[2] = (k == 0 ? -1.0 : 1.0) * getLength() / 2.;
				G4ThreeVector localPosition(local[0], local[1], local[2]);
				globalPosition = global2local.Inverse().TransformPoint(localPosition);
				global[0] = globalPosition.x();
				global[1] = globalPosition.y();
				global[2] = globalPosition.z();
				setGlobalPoint(global);
			}
		}
	}
}

G4VisAttributes* MRKElementField::getVisAttribute(G4String color)
{
	G4VisAttributes* p = nullptr;
	if(color.size() > 0 && (isdigit(color.c_str()[0]) || color.c_str()[0] == '.'))
	{
		G4double red = 0.0, green = 0.0, blue = 0.0;
		if(sscanf(color.c_str(), "%lf,%lf,%lf", &red, &green, &blue) == 3)
		{
			p = new G4VisAttributes(true, G4Color(red, green, blue));
		}
		else
		{
			G4cout << " Invalid color " << color << G4endl;
		}
	}

	if(!p) p = new G4VisAttributes(G4VisAttributes::Invisible);
	p->SetDaughtersInvisible(false);

	return p;
}
