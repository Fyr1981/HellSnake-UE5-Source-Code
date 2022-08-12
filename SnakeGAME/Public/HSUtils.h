#pragma once

class HSUtils
{
public:
    template <typename T>
    static T* GetHSPlayerComponent(AActor* Player)
    {
        if (!Player) { return nullptr; }
        const auto Component = Player->GetComponentByClass(T::StaticClass());
        return Cast<T>(Component);
    }
};
