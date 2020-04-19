#include <alsa/asoundlib.h>
#include <algorithm>

#include <iostream>

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace ftxui;

class AlsaVolumeController
{
    long min, max;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    snd_mixer_elem_t* elem;

public:
    AlsaVolumeController(const char *card, const char *selem_name) {
        snd_mixer_open(&handle, 0);
        snd_mixer_attach(handle, card);
        snd_mixer_selem_register(handle, NULL, NULL);
        snd_mixer_load(handle);

        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_index(sid, 0);
        snd_mixer_selem_id_set_name(sid, selem_name);
        elem = snd_mixer_find_selem(handle, sid);
        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    }
    ~AlsaVolumeController() {
        snd_mixer_close(handle);
    }
    AlsaVolumeController(const AlsaVolumeController &) = delete;

    void volume(float percentage)
    {
        long normalized = percentage * (max - min) + min; // to the scale and offset
        snd_mixer_selem_set_playback_volume_all(elem, normalized);
    }

    float volume()
    {
        long left, right;
        snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &left);
        snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_RIGHT, &right);
        float mid = (left + right) / 2;
        return (mid - min) / (max - min);
    }

    void mute_toggle()
    {
        snd_mixer_selem_set_playback_switch_all(elem, 0);
    }
};

class VolumeComponent : public Component {
public:
    VolumeComponent() {};

    bool OnEvent(Event event) override {
        if (event == Event::ArrowUp || event == Event::ArrowRight || event.values[0] == 'k' || event.values[0] == 'l') {
            on_increase();
        }
        else if (event == Event::ArrowDown || event == Event::ArrowLeft || event.values[0] == 'j' || event.values[0] == 'h') {
            on_decrease();
        }
        else if (event.values[0] == 'q') {
            on_escape();
        }
        else if (static_cast<char>(event.values[0]) == 'm') {
            on_mute();
        }
        else {
            return Component::OnEvent(event);
        }
        on_change();
        return true;
    }

    std::function<void()> on_increase = [](){};
    std::function<void()> on_decrease = [](){};
    std::function<void()> on_escape = [](){};
    std::function<void()> on_mute = [](){};
    std::function<void()> on_change = [](){};

    Element Render() override {
        return vbox(gauge(volume),
                    hbox(text(L"→,↑,k,l – vol up") | flex,
                         text(L"←,↓,h,j – vol down") | flex,
                         text(L"m – mute") | flex,
                         text(L"q – exit") | flex));
    }

    float volume;
};

int main(int argc, const char* argv[]) {
    AlsaVolumeController volctl("pulse", "Master");

    auto screen = ScreenInteractive::TerminalOutput();
    VolumeComponent component;
    component.volume = volctl.volume();
    component.on_increase = [&](){ volctl.volume(std::min(1.f, component.volume + 0.03f)); };
    component.on_decrease = [&](){ volctl.volume(std::max(0.f, component.volume - 0.03f)); };
    component.on_escape = screen.ExitLoopClosure();
    component.on_mute = [&](){ volctl.mute_toggle(); };
    component.on_change = [&](){ component.volume = volctl.volume();};
    screen.Loop(&component);

    return 0;
}
