#ifndef POWERBACKENDIFACE_H
#define POWERBACKENDIFACE_H

class PowerBackendIface
{

public:
    PowerBackendIface() {}
    virtual ~PowerBackendIface() {}

    virtual bool isOnBattery() const = 0;

    // try to declare signal
    virtual void batteryValueChanged(double value) = 0;

private:
};

#define PowerBackendInterface_iid "org.iviasclient.plugins.PowerBackendInterface"

Q_DECLARE_INTERFACE(PowerBackendIface, PowerBackendInterface_iid)

#endif // POWERBACKENDIFACE_H
