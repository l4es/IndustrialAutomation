function retval = test_var(temp_var)
    % This function tests whether the string that is passed as an
    % argument exists as a variable, and returns it if so.
    try
        % try to evaluate the parameter
        retval = evalin('base',temp_var);

        % The above is also successful for plain
        % numbers, so check for that
        if ~isnan(str2double(temp_var))
            retval = temp_var;
        end

    catch
        % Variable does not exist, so just use
        % its string value
        retval = temp_var;
    end
end
