// @(#)root/eve:$Id$
// Author: Matevz Tadel 2007

/*************************************************************************
 * Copyright (C) 1995-2007, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TEveSecondarySelectable.h"
#include "TEveElement.h"

#include "TGLSelectRecord.h"

//______________________________________________________________________________
//
// Semi-abstract interface for classes supporting secondary-selection.
//
// Element class that inherits from this, should also implement the
// following virtuals from TEveElement:
//    virtual void UnSelected();
//    virtual void UnHighlighted();
// and clear corresponding selection-set from there.
//
// To support tooltips for sub-elements, implement:
//    virtual TString TEveElement::GetHighlightTooltip();
// and return tooltip for the entry in the fHighlightedSet.
// There should always be a single entry there.
// See TEveDigitSet for an example.


ClassImp(TEveSecondarySelectable);

//______________________________________________________________________________
TEveSecondarySelectable::TEveSecondarySelectable() :
   fAlwaysSecSelect(kFALSE)
{
   // Constructor.
}

//______________________________________________________________________________
void TEveSecondarySelectable::ProcessGLSelection(TGLSelectRecord& rec)
{
   // Process secondary GL selection and populate selected set accordingly.

   if (rec.GetHighlight())
      ProcessGLSelectionInternal(rec, fHighlightedSet);
   else
      ProcessGLSelectionInternal(rec, fSelectedSet);
}

//______________________________________________________________________________
void TEveSecondarySelectable::ProcessGLSelectionInternal(TGLSelectRecord& rec,
                                                         SelectionSet_t& sset)
{
   // Process secondary GL selection and populate given set accordingly.

   Int_t id = (rec.GetN() > 1) ? (Int_t) rec.GetItem(1) : -1;

   if (sset.empty())
   {
      if (id >= 0)
      {
         sset.insert(id);
         rec.SetSecSelResult(TGLSelectRecord::kEnteringSelection);
      }
   }
   else
   {
      if (id >= 0)
      {
         if (rec.GetMultiple())
         {
            if (sset.find(id) == sset.end())
            {
               sset.insert(id);
               rec.SetSecSelResult(TGLSelectRecord::kModifyingInternalSelection);
            }
            else
            {
               sset.erase(id);
               if (sset.empty())
                  rec.SetSecSelResult(TGLSelectRecord::kLeavingSelection);
               else
                  rec.SetSecSelResult(TGLSelectRecord::kModifyingInternalSelection);
           }
         }
         else
         {
            if (sset.size() != 1 || sset.find(id) == sset.end())
            {
               sset.clear();
               sset.insert(id);
               rec.SetSecSelResult(TGLSelectRecord::kModifyingInternalSelection);
            }
         }
      }
      else
      {
         if (!rec.GetMultiple())
         {
            sset.clear();
            rec.SetSecSelResult(TGLSelectRecord::kLeavingSelection);
         }
      }
   }

   if (rec.GetSecSelResult() != TGLSelectRecord::kNone)
   {
      dynamic_cast<TEveElement*>(this)->StampColorSelection();
   }
}
