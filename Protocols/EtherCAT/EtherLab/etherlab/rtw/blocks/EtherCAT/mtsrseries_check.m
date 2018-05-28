function mtsrseries_check(section)
    en = cell2struct(...
            get_param(gcb,'MaskEnables'),...
            get_param(gcb,'MaskNames')...
            );
    values = cell2struct(...
            get_param(gcb,'MaskValues'),...
            get_param(gcb,'MaskNames')...
            );

    switch section
    %case 'output_type'
    %    % When not in vector output, no control and status outputs
    %    if strcmp(values.output_type, 'Vector Output')
    %        en.status = 'on';
    %    else
    %        en.status = 'off';
    %        set_param(gcb, 'status', 'off');
    %    end
    %case 'dtype'
    %    % Gain and Offset are only allowed with this Data Type
    %    if strcmp(values.dtype, 'Double with scale and offset');
    %        %en.scale = 'on';
    %        en.offset = 'on';
    %    else
    %        %en.scale = 'off';
    %        en.offset = 'off';
    %        %if ~strcmp(get_param(gcb, 'scale'), '1')
    %        %    set_param(gcb, 'scale', '1');
    %        %end
    %        if ~strcmp(get_param(gcb, 'offset'), '0')
    %            set_param(gcb, 'offset', '0');
    %        end
    %    end
    %    if strcmp(values.dtype, 'Raw bits')
    %        set_param(gcb,'filter','off');
    %        en.filter = 'off';
    %        en.tau = 'off';
    %    else
    %        en.filter = 'on';
    %    end
    case 'filter'
        if (strcmp(values.filter,'on'))
            en.tau = 'on';
        else
            en.tau = 'off';
        end
    end

    set_param(gcb,'MaskEnables',struct2cell(en));
