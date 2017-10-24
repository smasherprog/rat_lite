import { ClientSettings, Monitor } from '../lib/rat_lite';
export class ConnectModel{
    public Protocol: string;
    public Host: string;
    public Port: number;
}
export class OptionsModel{
    public ClientSettings: ClientSettings;
    public Monitors : Monitor[];
}