#include "effects/presets/effectchainpreset.h"

#include "effects/effectchainslot.h"
#include "effects/effectxmlelements.h"
#include "util/xml.h"

EffectChainPreset::EffectChainPreset() {
}

EffectChainPreset::EffectChainPreset(const QDomElement& chainElement) {
    // chainElement can come from untrusted input from the filesystem, so do not DEBUG_ASSERT
    if (chainElement.tagName() != EffectXml::Chain) {
        return;
    }
    if (!chainElement.hasChildNodes()) {
        return;
    }

    m_id = XmlParse::selectNodeQString(chainElement, EffectXml::ChainId);
    m_name = XmlParse::selectNodeQString(chainElement, EffectXml::ChainName);
    m_description = XmlParse::selectNodeQString(chainElement, EffectXml::ChainDescription);

    QString mixModeStr = XmlParse::selectNodeQString(chainElement, EffectXml::ChainMixMode);
    m_mixMode = EffectChainSlot::mixModeFromString(mixModeStr);

    m_dSuper = XmlParse::selectNodeDouble(chainElement, EffectXml::ChainSuperParameter);

    QDomElement effectsElement = XmlParse::selectElement(chainElement, EffectXml::EffectsRoot);
    QDomNodeList effectList = effectsElement.childNodes();

    for (int i = 0; i < effectList.count(); ++i) {
        QDomNode effectNode = effectList.at(i);
        if (effectNode.isElement()) {
            QDomElement effectElement = effectNode.toElement();
            EffectPresetPointer pPreset(new EffectPreset(effectElement));
            m_effectPresets.append(pPreset);
        }
    }
}

EffectChainPreset::EffectChainPreset(const EffectChainSlot* chain) {
    m_id = chain->id();
    m_name = chain->name();
    m_description = chain->description();
    m_mixMode = chain->mixMode();
    m_dSuper = chain->getSuperParameter();
    for (const auto& pEffectSlot : chain->getEffectSlots()) {
        m_effectPresets.append(EffectPresetPointer(new EffectPreset(pEffectSlot)));
    }
}

const QDomElement EffectChainPreset::toXml(QDomDocument* doc) const {
    QDomElement chainElement = doc->createElement(EffectXml::Chain);

    XmlParse::addElement(*doc, chainElement, EffectXml::ChainId, m_id);
    XmlParse::addElement(*doc, chainElement, EffectXml::ChainName, m_name);
    XmlParse::addElement(*doc, chainElement, EffectXml::ChainDescription, m_description);
    XmlParse::addElement(*doc, chainElement, EffectXml::ChainMixMode, EffectChainSlot::mixModeToString(m_mixMode));
    XmlParse::addElement(*doc, chainElement, EffectXml::ChainSuperParameter, QString::number(m_dSuper));

    QDomElement effectsElement = doc->createElement(EffectXml::EffectsRoot);
    for (const auto& pEffectPreset : m_effectPresets) {
        effectsElement.appendChild(pEffectPreset->toXml(doc));
    }
    chainElement.appendChild(effectsElement);

    return chainElement;
}

EffectChainPreset::~EffectChainPreset() {
}
