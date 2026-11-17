# Transformation

    Transformation je abstraktni rozhrani. Definuje jediny kontrak: Kazda trida, ketra umi popsat heometrickou transformaci, musi umet vratit matici 4x4. Samotna trida nic nevypocitava, nic neuklada a nic nevlastni. Slouzi jako spolecny typ, presk ktery engine pracuje s libovolnou transformaci (posunm rotacem skala, vl;astni matice, slozena transformace, HUD umisteni, atd...) jednotne.

## Popis implementace

    - Transformation nema rodice, takze nic nededi.
    - Mame zde virtyalni destruktor, ktery je nastaven na default. Objekty se totiz budou ukladat pres ukazatel na zakladni tridu Transformation*. Bez virtualniho destruktoru by se pri delete pres ukazatel nezavolal destruktor potomka.
    - virtual glm::mat4 getMatrix() slouzi jako vzor pro ostatni tridy ktere budou z Transformation dedit. Kazda trida tedy bude muset nastavit getMatrix(), ale zalezi jen na ni jak to pojme.
    - implementuje se dale v CustomMatrix, translate, Rotate, Scale, HudPlacement, DynamicRotate, TransformationComposite a dalsich.

# ICameraObserver

    ICameraObserver je abstraktni rozhrani pro navrhovy vzor Observer. Trida, ktera chce vedet, ze se kamera zmenila,z nej dedi a implementuje jedinou metodu onCameraChanged(). Kamera drzi seznam pozorovatelu a pri kazde zmene je obejde a zavole jejich metodu. Samotne rozhrani nic nevypocitaba, nic neuklada a nic nevlastni.
    Hlavnim konkretnim pozorovatelem je ShaderProgram, ketry pri zmenie kamery nahraje doi shaderu matice viewMatrix a projectionMatrix a pripadne cameraPosition.

## Popis implementace

    - Stejne jako Transformation zde mame virtuali default destruktor. Kamera drzi pozorovatele jako ICameraObserver*. Pri mazani pres ukazatel na zakladni tridu by bez virtualniho destruktoru nebyl zavolan destruktor potomka.
    - virtual void onCameraChanged(Camera* camera) slouzi ke spracovani oznameni. Parametr je ukazatel a ne kopie, aby pozorovatel videl aktualni stav kamery a nevytvarel zbytecnou kopii. Metoda nevraci nic a slouzi jen ke spracovani oznameni, to je jen jednosmerne.

## Vztah k ostatnim tridam

    - Camera: ta si drzi std::vector<ICameraObserver*> observers. Metoda attach(ICameraObserver*) prida pozorovatele do seznamu a notifyAll() zavola onCameraChanged(this) u kazdeho z nich. NotifyAll() se vola pri zmene pozice (setPosition(), rotate(), moveForward(), moveDown()), pri zmene Fovm pomeru stran, vzdalenosti dalsiho rezu a dalsich parametrech.
    - ShaderProgram: Dedi z ICameraObserver. Metoda onCameraChanged zavola use(), nahraje viewMatrix a projectionMatrix a pokud shader obsahuje uniform cameraPosition, nahraje i pozici okna.
    - Scene: Pri registraci programu zavola camera.attach(program) a hned pote program->oncxamerachanged(&camera). Tim se shader ihned synchronizuje s aktualnim stavem kamery, i kdyz se kamera jeste nehybe.

## Na co si dat pozor

    - Kamera nema metodu detach. V tomto projektu by jsem takovou funkci nevyuzil. Pokud je ShaderProgram znicen drive, nez kamera, zustane vv seznamu visicich ukazatelu a notifyAll() do nej povede k nedefinovanemu chovani.
    - NotifyAll() je verejne, takze ji muzeme zavolad odkudkoli a vsichni pozorovatele dostanou oznameni i bez skutecne zmeny kamery, to se muze naopak zase nekdy hodit.

# SceneClock

    SceneClock je herni/scenovy cas. Na rozdil od glfwGetTime() nebo od realneho casu, ktery prichazi z venku z hlavni smycky nese vlastni cas sceny, ktery lze zpomalit zrychlit nebo i zastavit. Animace, ktera maji reagovat na cas sceny (rotace, pulzovani nebo pohyb kyvadel), se tak daji ovladat jednnou tridou bez zasahu do ostatnich casti enginu. Trida opet nic nevlastni a nema vztah k OpenLg. Je to cista logika casu.

## Popis implementace

    - privatni promene: time vyjadruje celkovy cas sceny, vychozi je nula. delta je posledni prirustekm vychozi nula dokud neni zavolan advance. Speed ma default hodnotu 1 neboli 1x zrychleno. a paused ma zakladni hodnotu false, jelikoz na zacatku cas neni zastaveny.

    - void advance(float ralDeltaTime);
    - double getTime() const;
    - float getDelta() const;
    - void setPaused(bool value);
    - bool isPaused() const;
    - void setSpeed(float value);
    - bool getSpeed() const;

# Material

## Popis implementace

# CustomMatrix

## Popis implementace

# BasicModels

## Popis implementace

# Texture

## Popis implementace

# TextureOptions

## Popis implementace

# Shader

## Popis implementace

# ShaderProgram

## Popis implementace

# lit.vert

## Popis implementace

# phong.frag

## Popis implementace

# skybox.vert

## Popis implementace

# skybox.frag

## Popis implementace

# hud.vert

## Popis implementace

# hud.frag

## Popis implementace

# showcase.frag

## Popis implementace

# Light

## Popis implementace

# LightBuffer

## Popis implementace

# Camera

## Popis implementace

# Translate

## Popis implementace

# Rotate

## Popis implementace

# Scale

## Popis implementace

# TransformationComposite

## Popis implementace

# Oscillate

## Popis implementace

# LineMove

## Popis implementace

# DynamicRotate

## Popis implementace

# GrowScale

## Popis implementace

# HudPlacement

## Popis implementace

# CubeMap

## Popis implementace

# Skybox

## Popis implementace

# Model

## Popis implementace

# ModelLoader

## Popis implementace

# DrawableObject

## Popis implementace

# Resources

## Popis implementace

# Scene

## Popis implementace

# SceneInput

## Popis implementace

# MainInput

## Popis implementace

# ShowcaseInput

## Popis implementace

# TestSceneInput

## Popis implementace

# ShowcaseScene

## Popis implementace

# TestScene

## Popis implementace

# Application

## Popis implementace

# main.cpp

## Popis implementace
