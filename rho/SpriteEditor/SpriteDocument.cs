using System;
using System.Collections;
using System.Collections.Specialized;
using System.Drawing;
using rho.Documents;
using rho.Import;

namespace rho.SpriteEditor {
    public class SpriteDocument : AbstractDocument {
        public SpriteDocument()
        : base("") {
            sprite = new ikaSprite(16, 32);
        }

        public SpriteDocument(string name) : base(name) {
            sprite = new ikaSprite(name);
        }

        protected override void DoSave(string name) {
            sprite.Save(name);
        }

        public IList Frames {
            get { 
                return sprite.Frames; 
            }
        }

        public Size Size {
            get { 
                return sprite.Size; 
            }
        }

        public Rectangle HotSpot {
            get {
                return sprite.HotSpot;
            }
            set {
                sprite.HotSpot = value;
            }
        }

        public StringDictionary AnimScripts {
            get {
                return sprite.Scripts;
            }
        }

        public StringDictionary Metadata {
            get {
                return sprite.Metadata;
            }
        }

        ikaSprite sprite;
    }
}
